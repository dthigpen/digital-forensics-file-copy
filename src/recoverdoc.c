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

VOID DumpCompoundBinaryFileHeader(struct StructuredStorageHeader* header){
    UINT1 u1Index1;
    printf("Header Fields (hex)\n");
    printf("_abSig: ");
    for(u1Index1 = 0; u1Index1 < (sizeof(header->_abSig) / sizeof(UINT1)); u1Index1++){
        printf("%x", header->_abSig[u1Index1]);
    }
    printf("\n");
    printf("_clid: ");
    for(u1Index1 = 0; u1Index1 < (sizeof(header->_clid) / sizeof(UINT1)); u1Index1++){
        printf("%x", header->_clid[u1Index1]);
    }
    printf("\n");
    printf("_uMinorVersion: %x\n", header->_uMinorVersion); 
    printf("_uDllVersion: %x\n", header->_uDllVersion); 
    printf("_uByteOrder: %x\n", header->_uByteOrder); 
    printf("_uSectorShift: %x\n", header->_uSectorShift); 
    printf("_uMiniSectorShift: %x\n", header->_uMiniSectorShift); 
    printf("_usReserved: %x\n", header->_usReserved); 
    
    printf("_ulReserved1: %x\n", header->_ulReserved1); 
    printf("_csectDir: %x\n", header->_csectDir); 
    printf("_csectFat: %x\n", header->_csectFat); 
    printf("_sectDirStart: %x\n", header->_sectDirStart); 
    printf("_signature: %x\n", header->_signature); 
    printf("_ulMiniSectorCutoff: %x\n", header->_ulMiniSectorCutoff); 
    printf("_sectMiniFatStart: %x\n", header->_sectMiniFatStart); 
    printf("_csectMiniFat: %x\n", header->_csectMiniFat); 
    printf("_sectDifStart: %x\n", header->_sectDifStart); 
    printf("_csectDif: %x\n", header->_csectDif); 
}

UINT1 FindIndirectBlocks(UINT1 u1SearchFlags) {
    UINT8 u8GbdOffset;
    UINT4 u4GroupNo;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
    struct StructuredStorageHeader storageHeader;
    UINT4 u4DataBlockNumber;
    UINT1 blockBuffer[gu4BlockSize];
    UINT4 u4IndirectAddrBuffer[gu4BlockSize / 4];
    UINT4 u4NumBlockGroups;
    u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    UINT4 u4BlockCount = 0;
    float passingPercentOrderedAddresses = 0.65;
    
    if (u1SearchFlags != SCAN_FREE_BLOCKS && u1SearchFlags != SCAN_USED_BLOCKS && u1SearchFlags != SCAN_ALL_BLOCKS){
        u1SearchFlags = SCAN_ALL_BLOCKS;
    }
    printf("Total block groups: %d\n", u4NumBlockGroups);
	printf("Block size: %d\nAddresses Per Block: %d\n", gu4BlockSize, gu4BlockSize / 4);
    
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
                        UINT1 u1IsIndirectBlock = 1;
                        INT4 u4InOrderChecks = 0;
                        INT4 u4TotalAddressesToCheck = 256;
                        INT4 u4TotalAddressesChecked = 0;
                        
                        
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
                                u4LastAddress = u4IndirectAddrBuffer[u4AddressIndex];
                                u4InOrderChecks += 1;
                            }else {
                                // continue
                            }
                        }
                        float percentOrdered = (float)u4InOrderChecks / u4TotalAddressesChecked;
                        if(percentOrdered > passingPercentOrderedAddresses)
                        {
                            // counts how many blocks past this point, used during debugging
                            // if(u4DataBlockNumber >= 1553){
                            //     u4BlockCount += 1;
                            //     printf("%5u ", u4BlockCount);
                            // }
                            printf("Indirect Block at block: %u (%0.2f)\n", u4DataBlockNumber, percentOrdered);
                        }else{
                            
                        }
                    }
                }
            }
		}
		u4GroupNo++;
	}
	return 0;
}    

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
INT4 RecoverDocFindSignatures(UINT1 u1SearchFlags) {
	UINT8 u8GbdOffset;
    UINT4 u4GroupNo;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
    struct StructuredStorageHeader storageHeader;
    UINT4 u4DataBlockNumber;
    UINT1 blockBuffer[gu4BlockSize];
    UINT4 u4NumBlockGroups;
    u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    
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
                        InodeUtilReadDataBlock(u4DataBlockNumber, 0, blockBuffer, gu4BlockSize);
                        memset(&storageHeader, 0, sizeof(struct StructuredStorageHeader));
                        memcpy(&storageHeader, blockBuffer, 512);
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
                                if (u1MatchesSignatureValues(blockBuffer + 512, wordSubtypeValue, sizeof(wordSubtypeValue) / sizeof(UINT1)))
                                {
                                    printf("type: .doc\n");
                                }
                                else if (u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue1, sizeof(pptSubtypeValue1) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue2, sizeof(pptSubtypeValue2) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue3, sizeof(pptSubtypeValue3) / sizeof(UINT1))
                                || (u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue4a, sizeof(pptSubtypeValue4a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 518, pptSubtypeValue4b, sizeof(pptSubtypeValue4b) / sizeof(UINT1))))
                                    {
                                    printf("type: .ppt\n");
                                }
                                else if ((u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue1a, sizeof(xlsSubtypeValue1a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 517, xlsSubtypeValue1b, sizeof(xlsSubtypeValue1b) / sizeof(UINT1)))
                                || (u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue2a, sizeof(xlsSubtypeValue2a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 517, xlsSubtypeValue2b, sizeof(xlsSubtypeValue2b) / sizeof(UINT1)))
                                || u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue3, sizeof(xlsSubtypeValue3) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue4, sizeof(xlsSubtypeValue4) / sizeof(UINT1)))
                                {
                                    printf("type: .xls\n");
                                }
                            }
                            else
                            {
                                DumpCompoundBinaryFileHeader(&storageHeader);
                            }
                        }
                    }
                }
            }
		}
		u4GroupNo++;
	}
	return 0;
}
