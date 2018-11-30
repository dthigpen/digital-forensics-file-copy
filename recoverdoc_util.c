#include "recoverdocinc.h"
#include "copyfileinc.h"
#include "inodeinc.h"

// signatures reference http://sceweb.sce.uhcl.edu/abeysekera/itec3831/labs/FILE%20SIGNATURES%20TABLE.pdf
// Generic Document type header for .doc, .ppt, .xls
UINT1 abSigValue[8] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
UINT2 uMinorVersion;
UINT2 uDllVersionValue1 = 0x0003; 
UINT2 uDllVersionValue2 = 0x0004;
UINT2 uByteOrderValue1 = 0xFEFF;
UINT2 uByteOrderValue2 = 0xFFFE;
UINT2 uSectorShiftValue1 = 0x09;
UINT2 uSectorShiftValue2 = 0x0C; 
UINT2 uMiniSectorShiftValue = 0x0006;
UINT2 usReservedValue = 0;
UINT4 ulReserved1Value = 0;
UINT4 ulReserved2Value = 0;
const CHAR* headerAscii = "D0CF11E0A1B11AE1";

UINT1 wordSubtypeValue[4] = {0xEC, 0xA5, 0xC1, 0x00};
UINT1 xlsSubtypeValue1a[4] = {0xFD, 0xFF, 0xFF, 0xFF};
UINT1 xlsSubtypeValue1b[1] = {0x00};
UINT1 xlsSubtypeValue2a[4] = {0xFD, 0xFF, 0xFF, 0xFF};
UINT1 xlsSubtypeValue2b[1] = {0x00};
UINT1 xlsSubtypeValue3[8] = {0x09, 0x08, 0x10, 0x00, 0x00, 0x06, 0x05, 0x00};
UINT1 xlsSubtypeValue4[8] = {0x09, 0x08, 0x08, 0x00, 0x00, 0x05, 0x05, 0x00};
UINT1 pptSubtypeValue1[4] = {0xA0, 0x46, 0x1D, 0xF0};
UINT1 pptSubtypeValue2[4] = {0x00, 0x6E, 0x1E, 0xF0};
UINT1 pptSubtypeValue3[4] = {0x0F, 0x00, 0xE8, 0x03};
UINT1 pptSubtypeValue4a[4] = {0xFD, 0xFF, 0xFF, 0xFF};
UINT1 pptSubtypeValue4b[2] = {0x00, 0x00};
UINT1 wordFooterValue[14] = {0x57, 0x6F, 0x72, 0x64, 0x2E, 0x44, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E};
UINT1 pptFooterValue[37] = {0x50, 0x00, 0x6F, 0x00, 0x77, 0x00, 0x65, 0x00, 0x72, 0x00, 0x50, 0x00, 0x6F, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6F, 0x00, 0x63, 0x00, 0x75, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x74};
const CHAR* wordFooter =  "576F72642E446F63756D656E742E";
const CHAR* pptFooter = "50006F0077006500720050006F0069006E007400200044006F00630075006D0065006E0074";

UINT1 u1MatchesSignatureValues(UINT1 *u1ActualValues, UINT1 *u1CorrectValues, size_t size){
    UINT4 u4Index1;
    for(u4Index1 = 0; u4Index1 < size; u4Index1++){
        if (u1ActualValues[u4Index1] != u1CorrectValues[u4Index1]){
            return 0;
        }
    }
    return 1;
}
// searchFlag: 0 (search free data blocks), 1 (search used data blocks), 2 (search all data blocks)
INT4 RecoverDocFindMatches(UINT1 u1SearchFlags) {
	UINT8 u8GbdOffset;
    UINT4 u4GroupNo;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
    UINT4 u4DataBlockNumber;
    UINT4 u4NumBlockGroups;
    UINT4 *u4HeaderBlocks;
    UINT4 u4NumHeadersFound;
    UINT4 *u4IndirectBlocks;
    UINT4 u4NumIndirectsFound;
    UINT4 u4RecoveredFileCount;
    u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    
    u4NumHeadersFound = 0;
    u4NumIndirectsFound = 0;
    u4RecoveredFileCount = 0;
    u4HeaderBlocks = calloc(512, sizeof(UINT4));
    u4IndirectBlocks = calloc(512, sizeof(UINT4));
    

    if (u1SearchFlags != SCAN_FREE_BLOCKS && u1SearchFlags != SCAN_USED_BLOCKS && u1SearchFlags != SCAN_ALL_BLOCKS){
        u1SearchFlags = SCAN_ALL_BLOCKS;
    }
    printf("Total block groups: %d\n", u4NumBlockGroups);
	// Loop through block groups until a free datablock is found
    u4GroupNo = 0;
	while (u4GroupNo <= u4NumBlockGroups)
	{
		memset(&GroupDes, 0, sizeof(struct ext3_group_desc));
    		u8GbdOffset = gu4BlockSize + u4GroupNo * sizeof(struct ext3_group_desc);
    		if (InodeUtilReadDataOffset(u8GbdOffset, &GroupDes, sizeof(struct ext3_group_desc)) == INODE_FAILURE)
		{
    		    printf("ERROR: Failed to read Block group descriptor table %s:%d\n", __FILE__, __LINE__);
    		    return INODE_FAILURE;
    		}
		if (InodeUtilReadDataBlock(GroupDes.bg_block_bitmap, 0, pBuffer, gu4BlockSize) == INODE_FAILURE)
		{
			printf("ERROR: Failed to read Block %s:%d\n", __FILE__, __LINE__);
			return INODE_FAILURE;
		}
		printf("Scanning block group %d\n", u4GroupNo);
        for (u4ByteIndex = 0; u4ByteIndex < gu4BlockSize; u4ByteIndex++)
		{
			// Check each byte to see if it is all 1's (0xFF)
            if (pBuffer[u4ByteIndex] != BYTE || u1SearchFlags != SCAN_FREE_BLOCKS)
            {
                // If byte is not all 1's, find the first free bit
                for (u4BitIndex = 0; u4BitIndex < BYTE; u4BitIndex++)
                {
                    i1IsBitUsed = ((pBuffer[u4ByteIndex] >> u4BitIndex) & 1);                    
                    if (u1SearchFlags == SCAN_ALL_BLOCKS || i1IsBitUsed == u1SearchFlags)
                    {
                        u4DataBlockNumber = (u4GroupNo * sb.s_blocks_count) + ((u4ByteIndex * BYTE) + u4BitIndex + 1);
                        if(IsFileType(u4DataBlockNumber))
                        {
                            u4HeaderBlocks[u4NumHeadersFound] = u4DataBlockNumber;
                            u4NumHeadersFound++;
                        }
                        else if(u4DataBlockNumber > 550 && IsIndirect(u4DataBlockNumber))
                        {
                            u4IndirectBlocks[u4NumIndirectsFound] = u4DataBlockNumber;
                            u4NumIndirectsFound++;
                        }
                    }
                }
            }
		}
		u4GroupNo++;
	}

    UINT4 u4HeaderNumIndex;
    UINT4 u4IndirectNumIndex;
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];
    UINT4 u4TempBlockNumber;
    UINT4 u4LastFirstIndirectAddress;
    UINT4 u4LastSecondIndirectAddress;
    UINT4 u4ClaimedInodeNo;
    UINT8 u8Offset;
    struct ext3_inode NewInode;
    struct ext3_inode RootInode;
    struct ext3_dir_entry_2 NewDirectoryEntry;
    CHAR Name[] = "RecoveredDoc";
    
    u4LastFirstIndirectAddress = 0;
    u4LastSecondIndirectAddress = 0;
    
    // TODO Build new inode instead of copying an existing one and modifying it
    memset(&NewInode, 0, sizeof(NewInode));
    if (InodeUtilReadInode(ROOT_INODE, &NewInode) == INODE_FAILURE)
    {
        printf("ERROR: Failed to read Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
        return;
    }


    printf("File headers found: %4u\n", u4NumHeadersFound);
    printf("Indirects found: %4u\n", u4NumIndirectsFound);
    
    printf("Header  1st-Indirect 2st-Indirect\n");
    for(u4HeaderNumIndex = 0; u4HeaderNumIndex < u4NumHeadersFound; u4HeaderNumIndex++){
	UINT4 u4TotalBlocks = 12;
        UINT4 u4FileBlocks[15];
        u4FileBlocks[0] = u4HeaderBlocks[u4HeaderNumIndex];
        // TODO set the direct block nums or not and just use UINT4 vars for first block and the indirects
        u4FileBlocks[12] = 0;
        u4FileBlocks[13] = 0;
        u4FileBlocks[14] = 0;
        u4LastFirstIndirectAddress = 0;
        u4LastSecondIndirectAddress = 0;

        // Loop through each of the indirects to find the correct ones
        for(u4IndirectNumIndex = 0, u4DataBlockNumber = 0; u4IndirectNumIndex < u4NumIndirectsFound; u4IndirectNumIndex++){
            u4DataBlockNumber = u4IndirectBlocks[u4IndirectNumIndex];
            // printf("indirect at: %u\n",u4DataBlockNumber);
            memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
            if(u4FileBlocks[12] == 0){
                InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);   

                if(u4IndirectAddrBuffer[0] == u4HeaderBlocks[u4HeaderNumIndex] + 12){
                    u4FileBlocks[12] = u4DataBlockNumber;
                    u4LastFirstIndirectAddress = u4IndirectAddrBuffer[gu4BlockSize / 4 - 1];
		    if(u4LastFirstIndirectAddress == 0)
		    {
		        UINT4 u4IndirectAddrIndex;
		        for(u4IndirectAddrIndex = 0; u4IndirectAddrIndex < gu4BlockSize / 4; u4IndirectAddrIndex++)
		        {
			    if(u4IndirectAddrBuffer[u4IndirectAddrIndex] == 0)
			    {
				u4TotalBlocks += u4IndirectAddrIndex - 1;
				break;
			    }
		        }
		    }
		    break;
                }
	    }
        }
	if(u4LastFirstIndirectAddress > 0)
	{
		u4TotalBlocks += (gu4BlockSize / 4);
        for(u4IndirectNumIndex = 0, u4DataBlockNumber = 0; u4IndirectNumIndex < u4NumIndirectsFound; u4IndirectNumIndex++){
            u4DataBlockNumber = u4IndirectBlocks[u4IndirectNumIndex];
            memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
	
	    if(u4FileBlocks[13] == 0){
                InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
                
                if(IsIndirect(u4IndirectAddrBuffer[0])){
                    // Check to see if the first data block is comes right after the last on in the first indirect
                    u4TempBlockNumber = u4IndirectAddrBuffer[0];
                    memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
                    InodeUtilReadDataBlock(u4TempBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
                    if(u4IndirectAddrBuffer[0] == u4LastFirstIndirectAddress + 1){
                        u4FileBlocks[13] = u4DataBlockNumber;
                        // TODO make another buffer so we dont need to memset and read datablock again
                        memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
                        InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
                        // Read last block in second indirect, open it, then find the last data block address in it
                        if(u4IndirectAddrBuffer[gu4BlockSize / 4] > 0 && u4IndirectAddrBuffer[gu4BlockSize / 4] < sb.s_blocks_count){
                            u4TempBlockNumber = u4IndirectAddrBuffer[0];
                            memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
                            InodeUtilReadDataBlock(u4TempBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
                            u4LastSecondIndirectAddress = u4IndirectAddrBuffer[gu4BlockSize / 4 - 1];
                        }
						if(u4LastSecondIndirectAddress == 0)
							{
								UINT4 u4IndirectAddrIndex;
								for(u4IndirectAddrIndex = 0; u4IndirectAddrIndex < gu4BlockSize / 4; u4IndirectAddrIndex++)
								{
								if(u4IndirectAddrBuffer[u4IndirectAddrIndex] == 0)
								{
								u4TotalBlocks += (u4IndirectAddrIndex - 2)*(gu4BlockSize/4);
								memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));
								InodeUtilReadDataBlock(u4TempBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
								u4LastSecondIndirectAddress = u4IndirectAddrBuffer[gu4BlockSize / 4 - 1];
								for(u4IndirectAddrIndex = 0; u4IndirectAddrIndex < gu4BlockSize / 4; u4IndirectAddrIndex++)
								{
									if(u4IndirectAddrBudder[u4IndirectAddrIndex] == 0)
										u4TotalBlocks += u4IndirectAddrIndex - 1;
								}
								break;
								}
								}
							}
                    }
                }
                
            }else if(u4FileBlocks[14] == 0 && u4LastSecondIndirectAddress > 0){
                // TODO Get third indirect use last address of second indirect
            }
            
        }
        }
        printf("%6u  %11u %11u\n", u4FileBlocks[0], u4FileBlocks[12], u4FileBlocks[13]);

        // Claim new inode
        u4ClaimedInodeNo = 0;
        if (CopyFileUtilClaimFreeInode(&u4ClaimedInodeNo) == INODE_FAILURE)
        {
            printf("ERROR: Failed to claim new Inode: %s:%d\n", __FILE__, __LINE__);
            return;
        }
	NewInode.i_mode = 0x81FF;
    NewInode.i_blocks = u4TotalBlocks;
	NewInode.i_size = u4TotalBlocks * gu4BlockSize;
        UINT1 u1DirectBlockIndex;
        for(u1DirectBlockIndex = 0; u1DirectBlockIndex < 12; u1DirectBlockIndex++){
            NewInode.i_block[u1DirectBlockIndex] = u4HeaderBlocks[u4HeaderNumIndex] + u1DirectBlockIndex;
        }
        NewInode.i_block[12] = u4FileBlocks[12];
        NewInode.i_block[13] = u4FileBlocks[13];
        NewInode.i_block[14] = u4FileBlocks[14];
        
        CHAR filenameNumber[5];
        snprintf(filenameNumber, sizeof(filenameNumber), "%04u", ++u4RecoveredFileCount);
        UINT1 filetype; 
        INT4 filenameLength;
        filetype = IsFileType(u4HeaderBlocks[u4HeaderNumIndex]);

        if(filetype != FILE_TYPE_DOC && filetype != FILE_TYPE_PPT && filetype != FILE_TYPE_XLS){
            continue;
        }

        switch(filetype){
            case FILE_TYPE_DOC:
                filenameLength = strlen(Name) + strlen(filenameNumber) + strlen(EXT_DOC);
                break;
            case FILE_TYPE_PPT:
                filenameLength = strlen(Name) + strlen(filenameNumber) + strlen(EXT_PPT);
                break;
            case FILE_TYPE_XLS:
                filenameLength = strlen(Name) + strlen(filenameNumber) + strlen(EXT_XLS);
                break;
        }

        CHAR *fullFileName = malloc(filenameLength);
        strcpy(fullFileName, Name);
        strcat(fullFileName, filenameNumber);
        
        switch(filetype){
            case FILE_TYPE_DOC:
                strcat(fullFileName, EXT_DOC);        
                break;
            case FILE_TYPE_PPT:
                strcat(fullFileName, EXT_PPT);        
                break;
            case FILE_TYPE_XLS:
                strcat(fullFileName, EXT_XLS);        
                break;
        }

        printf("Recovered file: %s\n", fullFileName);
        // Get new inode offset
        u8Offset = 0;
        if (InodeUtilGetInodeOffset(u4ClaimedInodeNo, &u8Offset) == INODE_FAILURE)
        {
            printf("ERROR: Failed to get Inode offset: %d %s:%d\n", u4ClaimedInodeNo, __FILE__, __LINE__);
            return;
        }

        // Seek to new inode location and copy old inode info to it
        if (InodeUtilWriteDataOffset(u8Offset, &NewInode, sizeof(NewInode)) == INODE_FAILURE)
        {
            printf("ERROR: Failed to write Block %s:%d\n", __FILE__, __LINE__);
            return;
        }

            // Get root inode
        memset(&RootInode, 0, sizeof(RootInode));
        if (InodeUtilReadInode(ROOT_INODE, &RootInode) == INODE_FAILURE)
        {
            printf("ERROR: Failed to read Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
            return;
        }

        // Create new directory entry to put in root inode
        CopyFileUtilCreateDirectoryEntry(&NewDirectoryEntry, u4ClaimedInodeNo, fullFileName);

        // Add directory entry to root inode block 0
        if (InodeDirAddChildEntry(&NewDirectoryEntry, RootInode.i_block[0]) == INODE_FAILURE)
        {
            printf("ERROR: Failed to add child entry Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
            return;
        }
        free(fullFileName);
    }

    free(u4HeaderBlocks);
    free(u4IndirectBlocks);
	return 0;
}

UINT1 IsFileType(UINT4 u4DataBlockNumber) {
    UINT1 u1BlockBuffer[gu4BlockSize];
    InodeUtilReadDataBlock(u4DataBlockNumber, 0, u1BlockBuffer, gu4BlockSize);
    struct StructuredStorageHeader storageHeader;
    memset(&storageHeader, 0, sizeof(struct StructuredStorageHeader));
    memcpy(&storageHeader, u1BlockBuffer, 512);
    if (u1MatchesSignatureValues(storageHeader._abSig, abSigValue,sizeof(abSigValue) / sizeof(UINT1)))
    {
        printf("Windows Compound Binary File Format Found\n");
        printf("Data block number: %d\n", u4DataBlockNumber);
        if ((storageHeader._uDllVersion == uDllVersionValue1
                || storageHeader._uDllVersion == uDllVersionValue2)
            && storageHeader._usReserved == usReservedValue
            && storageHeader._ulReserved1 == ulReserved1Value
            && ((storageHeader._uDllVersion == uDllVersionValue1
                && storageHeader._uSectorShift == uSectorShiftValue1)
                || (storageHeader._uDllVersion == uDllVersionValue2
                && storageHeader._uSectorShift == uSectorShiftValue2))
            && storageHeader._uMiniSectorShift == uMiniSectorShiftValue)
            {
                if (u1MatchesSignatureValues(u1BlockBuffer + 512, wordSubtypeValue, sizeof(wordSubtypeValue) / sizeof(UINT1)))
                {
                    printf("type: .doc\n");
                    return FILE_TYPE_DOC;
                }
                else if (u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue1, sizeof(pptSubtypeValue1) / sizeof(UINT1))
                || u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue2, sizeof(pptSubtypeValue2) / sizeof(UINT1))
                || u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue3, sizeof(pptSubtypeValue3) / sizeof(UINT1))
                || (u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue4a, sizeof(pptSubtypeValue4a) / sizeof(UINT1))
                    && u1MatchesSignatureValues(u1BlockBuffer + 518, pptSubtypeValue4b, sizeof(pptSubtypeValue4b) / sizeof(UINT1))))
                    {
                    printf("type: .ppt\n");
                    return FILE_TYPE_PPT;
                }
                else if ((u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue1a, sizeof(xlsSubtypeValue1a) / sizeof(UINT1))
                    && u1MatchesSignatureValues(u1BlockBuffer + 517, xlsSubtypeValue1b, sizeof(xlsSubtypeValue1b) / sizeof(UINT1)))
                || (u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue2a, sizeof(xlsSubtypeValue2a) / sizeof(UINT1))
                    && u1MatchesSignatureValues(u1BlockBuffer + 517, xlsSubtypeValue2b, sizeof(xlsSubtypeValue2b) / sizeof(UINT1)))
                || u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue3, sizeof(xlsSubtypeValue3) / sizeof(UINT1))
                || u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue4, sizeof(xlsSubtypeValue4) / sizeof(UINT1)))
                {
                    printf("type: .xls\n");
                    return FILE_TYPE_XLS;
                }
            }
        return FILE_TYPE_CBF;
    }
    else 
    {
        return 0;
    }
}

UINT1 IsIndirect(UINT4 u4DataBlockNumber) {
    UINT4 u4TotalAddressesToCheck = gu4BlockSize / 4 / 4;
    float passingPercentOrderedAddresses = 0.75;
    UINT2 u2MinimumOrderedAddresses = 4;
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];
                        InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
UINT4 u4AddressIndex;
                        UINT4 u4LastAddress = u4IndirectAddrBuffer[0];
                        UINT4 u4LastOrderedAddress = u4LastAddress;
                        UINT4 u4InOrderChecks = 0;
                        UINT4 u4TotalAddressesChecked = 0;


                        for (u4AddressIndex = 1; u4AddressIndex < u4TotalAddressesToCheck; u4AddressIndex++)
                        {
                            u4TotalAddressesChecked = u4AddressIndex;
                            // if the two addresses are both 0 the block is not completely filled with addresses so break and look at percentage ordered thus far
                            if(u4IndirectAddrBuffer[u4AddressIndex] == 0 && u4LastAddress == 0)
                            {
                                break;
                            } // check that the address is greater than the last address
                            else if (u4IndirectAddrBuffer[u4AddressIndex] - u4LastAddress > 0)
                            {
                                // update the last address
                                u4LastOrderedAddress = u4LastAddress;
                                u4LastAddress = u4IndirectAddrBuffer[u4AddressIndex];
                                u4InOrderChecks += 1;
                            }else {
                                // continue
                            }

                        }
                        float percentOrdered = (float)u4InOrderChecks / u4TotalAddressesChecked;
                        
                        if(u4InOrderChecks > u2MinimumOrderedAddresses && percentOrdered > passingPercentOrderedAddresses)
                        {
                                // printf("%10u %10u %10u %6u %10u %10.2f\n", u4DataBlockNumber, u4IndirectAddrBuffer[0], u4LastOrderedAddress,u4AddressIndex, u4IndirectAddrBuffer[gu4BlockSize / 4 - 1], percentOrdered);
                            return 1;
                            
                        }
			return 0;
}
