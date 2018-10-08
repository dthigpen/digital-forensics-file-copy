/********************************************************************
 *
 * File         : copyfileproto.h
 * Author       : Team 7
 * Date created : October 1, 2018
 * Description  : This file contains all function prototypes 
 *
 *******************************************************************/

/*****************************************************************************/
/*                          copyfileutil.c                                  */
/*****************************************************************************/

void CopyFileUtilCreateDirectoryEntry(struct ext3_dir_entry_2* pDirEntry,UINT4 inode_num, UCHAR* name);
INT4 CopyFileUtilClaimFreeInode(UINT4* u4ClaimedInodeNo); // return inode number of found inode
VOID CopyFileInode(INT4 fd, UINT4 u4OldInodeNo, CHAR* name);
