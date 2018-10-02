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

#include "copyfileinc.h"
#include "inodeinc.h"

#define BYTE_SIZE 8
#define FULL_BYTE 0xFF

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
    CHAR *DEFAULT_DEV = "/dev/sdb1";
    CHAR *pDev;
    CHAR *filepath;
    INT4 fd, u4OldIndoNo, new_inode;


    if (argc == 1)
    {
        printf("Enter disk name as command line argument. e.g - sudo ./copyfile /dev/sda1 filepath\n");
        //return -1;
    }else if(argc == 2){
        printf("Using fefault device: %s", DEFAULT_DEV);
        pDev = DEFAULT_DEV;
        filepath = argv[2];
    }else{
        pDev = argv[1];
        filepath = argv[2];
        
        if(argc > 3)
        {
            printf("Only using the first two args.");
        }
    }
    /*
    printf("Args: dev: %s filename: %s",pDev,filepath);
    fd = open(filepath, O_RDWR);
    if(fd < 0){
        perror("Error opening file");
        return -1;
    }

    u4OldInodeNo = inode_from_filepath(fd);
    printf("Inode: %d\n", u4OldInodeNo);

    InodeInit(fd);
    InodeCopyFile(u4OldInodeNo);
    InodeInitExit();
    */
}

