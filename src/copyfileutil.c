#include "copyfileinc.h"
#include "inodeinc.h"

VOID InodeCopyFile(INT4 fd, UINT4 u4OldInodeNo, CHAR* name) {
    // Get old inode
    struct ext3_inode oldInode;

    memset(&oldInode, 0, sizeof(oldInode));

    if (InodeUtilReadInode(u4OldInodeNo, &oldInode) == INODE_FAILURE) {
        printf("ERROR: Failed to read Inode: %d %s:%d\n", u4OldInodeNo, __FILE__, __LINE__);
        return;
    }

    // Claim new inode
    UINT4 u4ClaimedInodeNo = 0;
    if(ClaimFreeInode(&u4ClaimedInodeNo) == INODE_FAILURE) {
        printf("ERROR: Failed to claim new Inode: %s:%d\n", __FILE__, __LINE__);
        return;
    }

    // Get new inode offset
    UINT8 pu8Offset = 0;
    if(InodeUtilGetInodeOffset(u4ClaimedInodeNo, &pu8Offset) == INODE_FAILURE) {
        printf("ERROR: Failed to get Inode offset: %d %s:%d\n", u4ClaimedInodeNo, __FILE__, __LINE__);
        return;

    }

    // Seek to new inode location and copy old inode info to it
    if(InodeUtilWriteDataOffset(pu8Offset, &oldInode, sizeof(oldInode)) == INODE_FAILURE) {
	printf("ERROR: Failed to write Block %s:%d\n", __FILE__, __LINE__);
	return;
    }

    // Get root inode
    struct ext3_inode rootInode;

    memset(&rootInode, 0, sizeof(rootInode));

    if (InodeUtilReadInode(ROOT_INODE, &rootInode) == INODE_FAILURE) {
        printf("ERROR: Failed to read Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
        return;
    }

    // Create new directory entry to put in root inode
    struct ext3_dir_entry_2 newDirectoryEntry;
    CreateDirectoryEntry(&newDirectoryEntry, u4ClaimedInodeNo, name);

    // Add directory entry to root inode block 0
    if(InodeDirAddChildEntry(&newDirectoryEntry, rootInode.i_block[0]) == INODE_FAILURE) {
	printf("ERROR: Failed to add child entry Inode: %d %s:%d\n", ROOT_INODE, __FILE__, __LINE__);
        return;
    }
    // ls command
    // open file
}

// Claims an inode and returns the block number and inode number
INT4 ClaimFreeInode(UINT4* u4ClaimedInodeNo) {
    	UINT4 u4GroupNo = 0;

	// Loop through block groups until a free inode is found
	while( 1 )
	{
		UINT4 i, j;
        	UINT1 pBuffer[gu4BlockSize];
		struct ext3_group_desc GroupDes;
		memset(&GroupDes, 0, sizeof(GroupDes));
    		UINT8 u8GbdOffset = gu4BlockSize + u4GroupNo * sizeof(struct ext3_group_desc);
    		if(InodeUtilReadDataOffset(u8GbdOffset, &GroupDes, sizeof(struct ext3_group_desc)) == INODE_FAILURE) {
    		    printf("ERROR: Failed to read Block group descriptor table %s:%d\n", __FILE__, __LINE__);
    		    return INODE_FAILURE;
    		}
		if(InodeUtilReadDataBlock(GroupDes.bg_inode_bitmap, 0, pBuffer, gu4BlockSize) == INODE_FAILURE) {
			printf("ERROR: Failed to read Block %s:%d\n", __FILE__, __LINE__);
			return INODE_FAILURE;
		}
		// This might need to be i < (sb.s_inodes_per_group / BYTE)?
		for (i = 0; i < gu4BlockSize; i++)
		{
			// Check each byte to see if it is all 1's (0xFF)
	    		if(pBuffer[i] != FULL_BYTE)
			{
				// If byte is not all 1's, find the first free bit
				for(j = 0; j < BYTE; j++)
				{
					INT1 bIsUsed = ((pBuffer[i] >> j) & 1);
					
					if(bIsUsed == 0)
					{
						// Write claimed bit into inode table
						pBuffer[i] = (pBuffer[i] | (1 << j));
						if(InodeUtilWriteDataBlock(GroupDes.bg_inode_bitmap, 0, pBuffer, gu4BlockSize) == INODE_FAILURE) {
							printf("ERROR: Failed to write Block %s:%d\n", __FILE__, __LINE__);
							return INODE_FAILURE;
						}
						
						*u4ClaimedInodeNo = (u4GroupNo * sb.s_inodes_per_group) + ((i * BYTE) + j + 1);
						return INODE_SUCCESS;
					}
				}
			}
		}
		u4GroupNo++;
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
INT4 InodeFromFilepath(INT4 fd)
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
	memcpy(pDirEntry->name, name, strlen(name));
	pDirEntry->rec_len = ceil((float)(pDirEntry->name_len + DIR_ENTRY_NAME_OFFSET) / 4) * 4;
}
