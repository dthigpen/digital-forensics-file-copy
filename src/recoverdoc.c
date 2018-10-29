#include "recoverdocinc.h"
#include "inodeinc.h"

// signatures reference http://sceweb.sce.uhcl.edu/abeysekera/itec3831/labs/FILE%20SIGNATURES%20TABLE.pdf
// Generic Document type header for .doc, .ppt, .xls
const CHAR* header = "D0CF11E0A1B11AE1";
const CHAR* wordSubtype = "ECA5C100";
const CHAR* wordFooter =  "576F72642E446F63756D656E742E";
const CHAR* xlsSubtype1a = "FDFFFFFF";
const CHAR* xlsSubtype1b = "00";
const CHAR* xlsSubtype2a = "FDFFFFFF";
const CHAR* xlsSubtype2b = "00";
const CHAR* xlsSubtype3 = "0908100000060500";
const CHAR* xlsSubtype4 = "0908080000050500";
const CHAR* pptSubtype1 = "A0461DF0";
const CHAR* pptSubtype2 = "006E1EF0";
const CHAR* pptSubtype3 = "0F00E803";
const CHAR* pptSubtype4a = "FDFFFFFF";
const CHAR* pptSubtype4b = "0000";
const CHAR* pptFooter = "50006F0077006500720050006F0069006E007400200044006F00630075006D0065006E0074";

INT4 RecoverDocFindSignatures() {
	UINT8 u8GbdOffset;
    UINT4 u4GroupNo = 0;
	UINT4 u4ByteIndex, u4BitIndex;
    UINT1 pBuffer[gu4BlockSize];
	INT1 i1IsBitUsed;
	struct ext3_group_desc GroupDes;
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
					if (1 || i1IsBitUsed == 0)
					{
                        u4DataBlockNumber = (u4GroupNo * sb.s_blocks_count) + ((u4ByteIndex * BYTE) + u4BitIndex + 1);
                            InodeUtilReadDataBlock(u4DataBlockNumber, 0, blockBuffer, gu4BlockSize);
                            if(HasHexSignatureAtOffset(blockBuffer, header, 0)) {
                                printf("Data block number: %d\n", u4DataBlockNumber);
                                printf("Office header found\n");
                                if(HasHexSignatureAtOffset(blockBuffer, wordSubtype , 512)){
                                    printf("type: .doc\n");
                                }else if(HasHexSignatureAtOffset(blockBuffer, pptSubtype1 , 512)
                                || HasHexSignatureAtOffset(blockBuffer, pptSubtype2 , 512)
                                || HasHexSignatureAtOffset(blockBuffer, pptSubtype3 , 512)
                                || (HasHexSignatureAtOffset(blockBuffer, pptSubtype4a , 512)
                                    && HasHexSignatureAtOffset(blockBuffer, pptSubtype4b , 518))){
                                    printf("type: .ppt\n");
                                }else if((HasHexSignatureAtOffset(blockBuffer, xlsSubtype1a , 512)
                                    && HasHexSignatureAtOffset(blockBuffer, xlsSubtype1b , 517))
                                || (HasHexSignatureAtOffset(blockBuffer, xlsSubtype2a , 512)
                                    && HasHexSignatureAtOffset(blockBuffer, xlsSubtype2b , 517))
                                || HasHexSignatureAtOffset(blockBuffer, xlsSubtype3 , 512)
                                || HasHexSignatureAtOffset(blockBuffer, xlsSubtype4 , 512)){
                                    printf("type: .xls\n");
                                }
                                printf("\n");
                            }
					}
				}
			}
		}
		u4GroupNo++;
	}
	return 0;
}

INT4 HasHexSignatureAtOffset(UINT1 *buffer, const CHAR *hexSigInASCII, UINT4 u4DecOffset) {
    UINT4 sigIndex;
    UINT1 isMatch = 1;
    for(sigIndex = 0; sigIndex < strlen(hexSigInASCII); sigIndex+=2, u4DecOffset++){
        CHAR hexstring[3];
        memcpy(hexstring, hexSigInASCII+sigIndex, 2);
        hexstring[2] = '\0';
        int number = (int)strtol(hexstring, NULL, 16);
        if(buffer[u4DecOffset] != number){
            isMatch = 0;
            break;
        }
    }
    return isMatch;
}