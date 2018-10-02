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

void CreateDirectoryEntry(struct ext3_dir_entry_2* pDirEntry,UINT4 inode_num, UCHAR* name);
INT4 InodeFromFilepath(int fd);
UINT4 ClaimFreeInode(); // return inode number of found inode
VOID InodeCopyFile(UINT4 fd, UINT4 u4OldInodeNo);
