#include "recoverdocinc.h"
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


/*UINT1 FindIndirectBlocks(UINT1 u1SearchFlags) {
    UINT8 u8GbdOffset;
    UINT4 u4GroupNo;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
    UINT4 u4DataBlockNumber;
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];
    UINT4 u4NumBlockGroups;
    u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    UINT4 u4BlockCount = 0;
    UINT4 u4TotalAddressesToCheck = gu4BlockSize / 4;
    float passingPercentOrderedAddresses = 0.75;
    UINT2 u2MinimumOrderedAddresses = 4;
    
    if (u1SearchFlags != SCAN_FREE_BLOCKS && u1SearchFlags != SCAN_USED_BLOCKS && u1SearchFlags != SCAN_ALL_BLOCKS){
        u1SearchFlags = SCAN_ALL_BLOCKS;
    }
    printf("Indirect search parameters:\nMinimum in order addresses: %u\n# Addresses to check per block: %u\nPassing Percent of ordered Addresses: %1.2f\n\n",u2MinimumOrderedAddresses,u4TotalAddressesToCheck,passingPercentOrderedAddresses);
    
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
        printf("%10s %10s %10s %6s %10s %10s\n", "Indirect", "1st Addr", "Last Used","# Used", "LastInBlk", "(ordered/checked)");
        for (u4ByteIndex = 0; u4ByteIndex < gu4BlockSize; u4ByteIndex++)
		{
			// Check each byte to see if it is all 1's (0xFF)
            if (1 || pBuffer[u4ByteIndex] != BYTE || u1SearchFlags != SCAN_FREE_BLOCKS)
            {
                // If byte is not all 1's, find the first free bit
                for (u4BitIndex = 0; u4BitIndex < BYTE; u4BitIndex++)
                {
                    i1IsBitUsed = ((pBuffer[u4ByteIndex] >> u4BitIndex) & 1);                    
                    if (1 || u1SearchFlags == SCAN_ALL_BLOCKS || i1IsBitUsed == u1SearchFlags)
                    {
                        u4DataBlockNumber = (u4GroupNo * sb.s_blocks_count) + ((u4ByteIndex * BYTE) + u4BitIndex + 1);
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
                            if(u4IndirectAddrBuffer[0] < sb.s_blocks_count  && u4LastOrderedAddress < sb.s_blocks_count){
                                // counts how many blocks past this point, used during debugging
                                // if(u4DataBlockNumber >= 1553){
                                //     u4BlockCount += 1;
                                //     printf("%5u ", u4BlockCount);
                                // }
                                // printf("Indirect block: %5u (%0.2f) First Addr: %6u  [%4u] Addr: %6u Last Addr: %6u\n", u4DataBlockNumber, percentOrdered, u4IndirectAddrBuffer[0],u4AddressIndex -1, u4LastOrderedAddress, u4IndirectAddrBuffer[gu4BlockSize / 4 - 1]);
                                printf("%10u %10u %10u %6u %10u %10.2f\n", u4DataBlockNumber, u4IndirectAddrBuffer[0], u4LastOrderedAddress,u4AddressIndex, u4IndirectAddrBuffer[gu4BlockSize / 4 - 1], percentOrdered);
                            }
                        }
                    }
                }
            }
		}
		u4GroupNo++;
	}
	return 0;
}    */

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
    
    u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    
    u4NumHeadersFound = 0;
    u4NumIndirectsFound = 0;
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
                            // TODO: append to array of files
                            u4HeaderBlocks[u4NumHeadersFound] = u4DataBlockNumber;
                            u4NumHeadersFound++;
                            // printf("Got here 1\n");
                        }
                        else if(IsIndirect(u4DataBlockNumber))
                        {
                            // TODO: append to array of indirects
                            u4IndirectBlocks[u4NumIndirectsFound] = u4DataBlockNumber;
                            u4NumIndirectsFound++;
                            // printf("Got here 2\n");
                        }
                    }
                }
            }
		}
		u4GroupNo++;
	}
	// TODO:
	// for each in array of file block numbers as f
    UINT4 u4HeaderNumIndex;
    UINT4 u4IndirectNumIndex;
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];
    UINT4 u4SecondIndirectAddrBuffer[gu4BlockSize / 4];
    UINT4 u4TempBlockNumber;
    UINT4 u4LastFirstIndirectAddress;
    UINT4 u4LastSecondIndirectAddress;
    UINT4 u4BlockIndex;
    u4LastFirstIndirectAddress = 0;
    u4LastSecondIndirectAddress = 0;

    printf("File headers found: %4u\n", u4NumHeadersFound);
    printf("Indirects found: %4u\n", u4NumIndirectsFound);
    printf("Header  1st-Indirect 2st-Indirect\n");
    for(u4HeaderNumIndex = 0; u4HeaderNumIndex < u4NumHeadersFound; u4HeaderNumIndex++){
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
            memset(&u4IndirectAddrBuffer, 0, sizeof(u4IndirectAddrBuffer));

            if(u4FileBlocks[12] == 0){
                InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);   

                if(u4IndirectAddrBuffer[0] == u4HeaderBlocks[u4HeaderNumIndex] + 12){
                    u4FileBlocks[12] = u4DataBlockNumber;
                    u4LastFirstIndirectAddress = u4IndirectAddrBuffer[gu4BlockSize / 4 - 1];
                }
            }else if(u4FileBlocks[13] == 0 && u4LastFirstIndirectAddress > 0){
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
                    }
                }
                
            }else if(u4FileBlocks[14] == 0 && u4LastSecondIndirectAddress > 0){
                // TODO Get third indirect use last address of second indirect
            }
            
        }
        printf("%6u  %11u %11u\n", u4FileBlocks[0], u4FileBlocks[12], u4FileBlocks[13]);
        // TODO Start inode steps as listed in the pseudocode
        
    }

	//	Add block number f + next 11 sequential block numbers to array block_nums
	// 	for each in array of indirects as i
	//		if f + 12 == i[0]
	//			This indirect belongs to this file
	//			Add this indirect to block_nums
	//	claim free inode
	//	create inode entry with block_nums as the block numbers
	//	write new inode entry at position that was claimed
	//	create new directory entry in root directory
	//	give new directory entry the file extension in the name (.doc, .ppt, .xls)
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
                                }
                                else if (u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue1, sizeof(pptSubtypeValue1) / sizeof(UINT1))
                                || u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue2, sizeof(pptSubtypeValue2) / sizeof(UINT1))
                                || u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue3, sizeof(pptSubtypeValue3) / sizeof(UINT1))
                                || (u1MatchesSignatureValues(u1BlockBuffer + 512, pptSubtypeValue4a, sizeof(pptSubtypeValue4a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(u1BlockBuffer + 518, pptSubtypeValue4b, sizeof(pptSubtypeValue4b) / sizeof(UINT1))))
                                    {
                                    printf("type: .ppt\n");
                                }
                                else if ((u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue1a, sizeof(xlsSubtypeValue1a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(u1BlockBuffer + 517, xlsSubtypeValue1b, sizeof(xlsSubtypeValue1b) / sizeof(UINT1)))
                                || (u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue2a, sizeof(xlsSubtypeValue2a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(u1BlockBuffer + 517, xlsSubtypeValue2b, sizeof(xlsSubtypeValue2b) / sizeof(UINT1)))
                                || u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue3, sizeof(xlsSubtypeValue3) / sizeof(UINT1))
                                || u1MatchesSignatureValues(u1BlockBuffer + 512, xlsSubtypeValue4, sizeof(xlsSubtypeValue4) / sizeof(UINT1)))
                                {
                                    printf("type: .xls\n");
                                }
                            }
                            else
                            {
                            }
			    return 1;
                        } else {
				return 0;
			}
}

UINT1 IsSecondIndirect(UINT4 u4DataBlockNumber){
    UINT1 u1IsIndirect;
    UINT4 u4BlockIndex;
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];

    if(IsIndirect(u4DataBlockNumber)){
        u1IsIndirect = 1;
        InodeUtilReadDataBlock(u4DataBlockNumber, 0, u4IndirectAddrBuffer, gu4BlockSize);
        for(u4BlockIndex = 0; u4BlockIndex < gu4BlockSize / 4; u4BlockIndex++){            
            if(u4IndirectAddrBuffer[u4BlockIndex] > 0  && u4IndirectAddrBuffer[u4BlockIndex] < sb.s_blocks_count && IsIndirect(u4IndirectAddrBuffer[u4BlockIndex]) == 0){
                u1IsIndirect = 0;
                break;
            }
        }
        if(u1IsIndirect){
            return 1;
        }
    }

    return 0;
}

UINT1 IsIndirect(UINT4 u4DataBlockNumber) {
    UINT4 u4TotalAddressesToCheck = gu4BlockSize / 4;
    float passingPercentOrderedAddresses = 0.75;
    UINT2 u2MinimumOrderedAddresses = 4;
    UINT4 u4IndirectAddrBuffer[u4TotalAddressesToCheck];
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
                            if(u4IndirectAddrBuffer[0] < sb.s_blocks_count  && u4LastOrderedAddress < sb.s_blocks_count){
                                // printf("%10u %10u %10u %6u %10u %10.2f\n", u4DataBlockNumber, u4IndirectAddrBuffer[0], u4LastOrderedAddress,u4AddressIndex, u4IndirectAddrBuffer[gu4BlockSize / 4 - 1], percentOrdered);
				return 1;
                            }
                        }
			return 0;
}
