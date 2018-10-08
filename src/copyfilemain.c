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
    CHAR *Partition;
    CHAR *InodeString;
    CHAR *FileName;
    INT4 fd;
    UINT4 u4OldInodeNo;

    if (argc < 4)
    {
        printf("Example usage: sudo ./copyfile <partition> <fileinode> <newfilename>\n");
        return;
    } else if (argc >= 4) {
	Partition = argv[1];
	InodeString = argv[2];
	FileName = argv[3];
    }

    printf("Args: partition: %s inode: %s, filename: %s\n", Partition, InodeString, FileName);
    fd = open(Partition, O_RDWR);
    if(fd < 0){
        perror("Error opening file");
        return;
    }

    u4OldInodeNo = atoi(InodeString);

    InodeInit(fd);
    CopyFileUtilCopyFile(fd, u4OldInodeNo, FileName);
    InodeInitExit();
}

