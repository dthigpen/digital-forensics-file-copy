/*******************************************************************
 *
 * File         : copyfilemain.c
 * Author       : 
 * Date created : September 29, 2018
 * Description  : This program copies a file to the root directory by
 *                allocating a new inode and copying over all
 *                information from the old inode and adding a new
 *                entry to the root directory for the file.
 *
 *******************************************************************/

#define _LARGEFILE64_SOURCE
//TODO: should add this define in suitable location
#include "copyfileinc.h"
#include "inodeinc.h"

/*! *************************************************************************
 * Function Name : main 
 *
 * Description   : This is the main() function for the inode program.
 *
 * Input         : argc - total no. of command line arguments
 *                 argv - command line arguments
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID main(INT4 argc, CHAR** argv )
{
    CHAR *pDev;
    CHAR *cInode;
    CHAR *name;
    INT4 fd;
    UINT4 u4OldInodeNo;

    if (argc < 4)
    {
        printf("usage example: sudo ./copyfile /dev/sdX1 inodeNumToCopy newFileName\n");
        //return -1;
    } else if (argc >= 4)
    {
        pDev = argv[1];
        cInode = argv[2];
        name = argv[3];
    }

    fd = open(pDev, O_RDWR);
    if (fd < 0)
    {
        perror("Error opening file");
        return;
    }

    u4OldInodeNo = atoi(cInode);

    InodeInit(fd);
    CopyFileInode(fd, u4OldInodeNo, name);
    InodeInitExit();
}

