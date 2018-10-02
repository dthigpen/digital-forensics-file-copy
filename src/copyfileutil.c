#include "copyfileinc.h"
#include "inodeinc.h"

VOID InodeCopyFile(UINT4 fd, UINT4 u4OldInodeNo) {
    // Get old inode
    struct ext3_inode oldInode;

    memset(&oldInode, 0, sizeof(oldInode));

    if (InodeUtilReadInode(u4OldInodeNo, &oldInode) == INODE_FAILURE) {
        printf("ERROR: Failed to read Inode: %d %s:%d\n", u4OldInodeNo, __FILE__, __LINE__);
        return -1;
    }

    // Claim new inode
    UINT4 u4ClaimedInodeNum = ClaimFreeInode();

    // Get new inode offset
    UINT8 pu8Offset = 0;
    if(InodeUtilGetInodeOffset(u4ClaimedInodeNum, &pu8Offset) == INODE_FAILURE) {
        printf("ERROR: Failed to get Inode offset: %d %s:%d\n", u4ClaimedInodeNum, __FILE__, __LINE__);
        return -1;

    }

    // Seek to new inode location and copy old inode info to it
    fseek64(fd, pu8Offset, SEEK_SET);
    write(fd, &oldInode, sizeof(oldInode));

    // Get root inode
    struct ext3_inode rootInode;

    memset(&rootInode, 0, sizeof(rootInode));

    if (InodeUtilReadInode(ROOT_INODE, &rootInode) == INODE_FAILURE) {
        printf("ERROR: Failed to read Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
        return -1;
    }

    // Create new directory entry to put in root inode
    struct ext3_dir_entry_2 newDirectoryEntry;
    CreateDirectoryEntry(&newDirectoryEntry, u4ClaimedInodeNum, "some name");

    // Add directory entry to root inode block 0
    if(InodeDirAddChildEntry(&newDirectoryEntry, rootInode.i_block[0]) == INODE_FAILURE) {
	printf("ERROR: Failed to add child entry Inode: %d %s:%d\n", u4Index, __FILE__, __LINE__);
        return -1;
    }
    // ls command
    // open file
}

// Claims an inode and returns the block number and inode number
UINT4 ClaimFreeInode() {
	UINT4 blockGroupNbr = 0;
	UINT4 u4BlockSize = 1024 << sb.s_log_block_size;
	struct ext3_group_desc GroupDes;

	// Loop through block groups until a free inode is found
	while( 1 )
	{
		memset(&GroupDes, 0, sizeof(GroupDes));
		INT4 retVal;
		UINT1 i;
        UINT1 b[u4BlockSize];
		UINT8 u8GbdOffset = 0;
		
    	UINT4 u4GroupNo = 0;

		u8GbdOffset = u4BlockSize + u4GroupNo * sizeof(struct ext3_group_desc);
		if(InodeUtilReadDataOffset(u8GbdOffset, &GroupDes, sizeof(struct ext3_group_desc) == INODE_FAILURE)) {
			printf("ERROR: Failed to read Block group descriptor table %s:%d\n", __FILE__, __LINE__);
			return INODE_FAILURE;
		}
	        // Use group 3's function to get block group at blockGroupNbr
		// UINT4 inodeBitmapBlock = Use group 3's function to get inode bitmap block
		// UINT4 inodeTableBlock = Use group 3's function to get inode table block
		// UINT4 inodesPerBlockGroup = Use superblock;
		// fseek64 to inodeBitmapBlock;
		
		// Read inode bitmap into memory
        	retVal = read(fd, b, u4BlockSize);
        	if ( retVal <= 0 )
        	{
                	fprintf(stderr, "unable to read disk, retVal = %d\n", retVal );
                	exit(1);
        	}
		// This might need to be i < (sb.s_inodes_per_group / BYTE_SIZE)?
		for (i = 0; i < u4BlockSize; i++)
		{
			// Check each byte to see if it is all 1's (0xFF)
	    		if(b[i] != FULL_BYTE)
			{
				// If byte is not all 1's, find the first free bit
				for(j = 0; j < BYTE_SIZE; j++)
				{
					INT1 bIsUsed = ((b[i] >> j) & 1);
					
					if(bIsUsed == 0)
					{
						// Seek to this byte
						fseek(fd, i, SEEK_CUR);
						// Claim bit j in this byte
						INT1 claimed = (b[i] | (1 << j));
						// Write claimed bit into inode table
						write(fd, &claimed, 1);
						
						return (blockGroupNbr * sb.s_inodes_per_group) + ((i * BYTE_SIZE) + j);
					}
				}
			}
		}
		blockGroupNbr++;
	}
	return 0;
}

/*! *************************************************************************
 * Function Name :  inode_from_filepath
 *
 * Description   : This function returns the inode of a particular file.
 *
 * Input         : fd - the file descriptor of the file to get the inode for.
 *
 * Output        : None
 * 
 * Returns       : INT4 - the inode number of the file that was input
 *
 **************************************************************************/
INT4 InodeFromFilepath(int fd)
{
    struct stat file_stat;
    INT4 ret;
    ret = fstat(fd, &file_stat);
    if(ret < 0){
        perror("Error getting file stats");
    }
    return file_stat.st_ino;
    
}

void CreateDirectoryEntry(struct ext3_dir_entry_2* pDirEntry, UINT4 inode_num, UCHAR* name)
{
	pDirEntry->inode = inode_num;
	pDirEntry->file_type = EXT3_FT_REG_FILE;
	pDirEntry->name_len = strlen(name);
	memset(pDirEntry->name, 0, EXT3_NAME_LEN);
	pDirEntry->name = name;
	pDirEntry->rec_len = DIR_REC_LEN(pDirEntry);
}
