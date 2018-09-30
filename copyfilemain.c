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

#include <sys/stat.h>



INT4 inodeFromFilepath(int fd);
void getFreeInode(); // return block number of inode
void writeInodeDataBlocks();

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
    INT4 fd, old_inode, new_inode;


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
    
    printf("Args: dev: %s filename: %s",pDev,filepath);
    fd = open(filepath, O_RDONLY);
    if(fd < 0){
        perror("Error opening file");
        return -1;
    }

    old_inode = inode_from_filepath(fd);
    printf("Inode: %d\n", old_inode);
    
    int blocknum = getFreeInode();
    // fseek64 to blocknum
    // Get inode struct for old file inode number from group 6
    // Copy meta data from old inode struct to new inode using fwrite
    // writeInodeDataBlocks(old, new);
    // Get inode struct for root directory inode number group 5
    // Open data blocks for directory inode struct
    // Iterate through data blocks then add directory entry (filename, inode number)
    // ls command
    // open file
}

void getFreeInode(){

    // Loop through the block groups for free inodes (group 3)
    // Go to inode bitmap given block number (group 4)
    // Read block for first bit that is 0, or unused

    // return this index as the block that contains the inode
}

// parameters: old_inode_struct, new_inode_struct
void writeInodeDataBlocks(){

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
INT4 inode_from_filepath(int fd)
{
    struct stat file_stat;
    INT4 ret;
    ret = fstat(fd, &file_stat);
    if(ret < 0){
        perror("Error getting file stats");
    }
    return file_stat.st_ino;
    
}






