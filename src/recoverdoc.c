#include "recoverdocinc.h"
#include "inodeinc.h"

// signatures reference http://sceweb.sce.uhcl.edu/abeysekera/itec3831/labs/FILE%20SIGNATURES%20TABLE.pdf
// Generic Document type header for .doc, .ppt, .xls
UINT1 abSigValue[8] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};
UINT2 uMinorVersion; // could be anything
UINT2 uDllVersionValue1 = 0x0003; 
UINT2 uDllVersionValue2 = 0x0004;
UINT2 uByteOrderValue1 = 0xFEFF;
UINT2 uByteOrderValue2 = 0xFFFE;
UINT2 uSectorShiftValue1 = 0x09; // typically 9 or 12 depending on major version
UINT2 uSectorShiftValue2 = 0x0C; 
UINT2 uMiniSectorShiftValue; // typically 12
UINT2 usReservedValue = 0;
UINT4 ulReserved1Value = 0;
UINT4 ulReserved2Value = 0;
const CHAR* headerAscii = "D0CF11E0A1B11AE1";

UINT1 wordSubtypeValue[4] = {0xEC, 0xA5, 0xC1, 0x00};
UINT1 xlsSubtypeValue1a[4] = {0xFD, 0xFF, 0xFF, 0xFF};
UINT1 xlsSubtypeValue1b[1] = {0x00};
UINT1 xlsSubtypeValue2a[4] = {0xFD, 0xFF, 0xFF, 0xFF};
UINT1 xlsSubtypeValue2b[4] = {0x00};
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

struct StructuredStorageHeader {
    UINT1 _abSig[8]; // [offset from start in bytes, length in bytes]
    // [000H,08] {0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1} for current version,
    // was {0x0e, 0x11, 0xfc, 0x0d, 0xd0, 0xcf, 0x11, 0xe0} on old, beta 2 files (late ’92)
    UINT1 _clid[16]; // [008H,16] class id (set with WriteClassStg, retrieved with GetClassFile/ReadClassStg)
    UINT2 _uMinorVersion; // [018H,02] minor version of the format: 33 is written by reference implementation
    UINT2 _uDllVersion; // [01AH,02] major version of the dll/format: 3 is written by reference implementation
    UINT2 _uByteOrder; // [01CH,02] 0xFFFE: indicates Intel byte-ordering
    UINT2 _uSectorShift; // [01EH,02] size of sectors in power-of-two (typically 9, indicating 512-byte sectors)
    UINT2 _uMiniSectorShift; // [020H,02] size of mini-sectors in power-of-two (typically 6, indicating 64-byte mini-sectors)
    UINT2 _usReserved; // [022H,02] reserved, must be zero
    UINT4 _ulReserved1; // [024H,04] reserved, must be zero
    UINT4 _ulReserved2; // [028H,04] reserved, must be zero
    UINT4 _csectFat; // [02CH,04] number of SECTs in the FAT chain
    UINT4 _sectDirStart; // [030H,04] first SECT in the Directory chain
    UINT4 _signature; // [034H,04] signature used for transactionin: must be zero. 
    UINT4 _ulMiniSectorCutoff; // [038H,04] maximum size for mini-streams: typically 4096 bytes
    UINT4 _sectMiniFatStart; // [03CH,04] first SECT in the mini-FAT chain
    UINT4 _csectMiniFat; // [040H,04] number of SECTs in the mini-FAT chain
    UINT4 _sectDifStart; // [044H,04] first SECT in the DIF chain
    UINT4 _csectDif; // [048H,04] number of SECTs in the DIF chain
    UINT4 _sectFat[109]; // [04CH,436] the SECTs of the first 109 FAT sectors
};


UINT1 u1MatchesSignatureValues(UINT1 *u1ActualValues, UINT1 *u1CorrectValues, size_t size){
    // size_t size = sizeof(u1CorrectValues) / sizeof(UINT1);
    UINT4 i;
    for(i = 0; i < size; i++){
        if(u1ActualValues[i] != u1CorrectValues[i]){
            return 0;
        }
    }
    return 1;
}

INT4 RecoverDocFindSignatures() {
	UINT8 u8GbdOffset;
    UINT4 u4GroupNo = 0;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
    struct StructuredStorageHeader storageHeader;
    UINT4 u4DataBlockNumber;
    UINT1 blockBuffer[gu4BlockSize];
    UINT4 u4NumBlockGroups = sb.s_blocks_count / sb.s_blocks_per_group;
    printf("Total block groups: %d\n", u4NumBlockGroups);
	// Loop through block groups until a free datablock is found
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
	    		if (pBuffer[u4ByteIndex] != BYTE)
			{
				// If byte is not all 1's, find the first free bit
				for (u4BitIndex = 0; u4BitIndex < BYTE; u4BitIndex++)
				{
					i1IsBitUsed = ((pBuffer[u4ByteIndex] >> u4BitIndex) & 1);                    
					if (i1IsBitUsed == 0)
					{
                        u4DataBlockNumber = (u4GroupNo * sb.s_blocks_count) + ((u4ByteIndex * BYTE) + u4BitIndex + 1);
                            InodeUtilReadDataBlock(u4DataBlockNumber, 0, blockBuffer, gu4BlockSize);
                            memset(&storageHeader, 0, sizeof(struct StructuredStorageHeader));
                            memcpy(&storageHeader, blockBuffer, 512);
                            if(u1MatchesSignatureValues(storageHeader._abSig, abSigValue,sizeof(abSigValue) / sizeof(UINT1))
                            && (storageHeader._uDllVersion == uDllVersionValue1
                                || storageHeader._uDllVersion == uDllVersionValue2)
                            && storageHeader._usReserved == usReservedValue
                            && storageHeader._ulReserved1 == ulReserved1Value
                            && storageHeader._ulReserved2 == ulReserved2Value
                            && ((storageHeader._uDllVersion == uDllVersionValue1
                                && storageHeader._uSectorShift == uSectorShiftValue1)
                                || (storageHeader._uDllVersion == uDllVersionValue2
                                && storageHeader._uSectorShift == uSectorShiftValue2)))
                                {
                                printf("Windows Compound Binary File Format Found\n");
                                printf("Data block number: %d\n", u4DataBlockNumber);
                                if(u1MatchesSignatureValues(blockBuffer + 512, wordSubtypeValue, sizeof(wordSubtypeValue) / sizeof(UINT1))){
                                    printf("type: .doc\n");
                                }
                                else if(u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue1, sizeof(pptSubtypeValue1) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue2, sizeof(pptSubtypeValue2) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue3, sizeof(pptSubtypeValue3) / sizeof(UINT1))
                                || (u1MatchesSignatureValues(blockBuffer + 512, pptSubtypeValue4a, sizeof(pptSubtypeValue4a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 518, pptSubtypeValue4b, sizeof(pptSubtypeValue4b) / sizeof(UINT1)))){
                                    printf("type: .ppt\n");
                                }else if((u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue1a, sizeof(xlsSubtypeValue1a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 517, xlsSubtypeValue1b, sizeof(xlsSubtypeValue1b) / sizeof(UINT1)))
                                || (u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue2a, sizeof(xlsSubtypeValue2a) / sizeof(UINT1))
                                    && u1MatchesSignatureValues(blockBuffer + 517, xlsSubtypeValue2b, sizeof(xlsSubtypeValue2b) / sizeof(UINT1)))
                                || u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue3, sizeof(xlsSubtypeValue3) / sizeof(UINT1))
                                || u1MatchesSignatureValues(blockBuffer + 512, xlsSubtypeValue4, sizeof(xlsSubtypeValue4) / sizeof(UINT1))){
                                    printf("type: .xls\n");
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
