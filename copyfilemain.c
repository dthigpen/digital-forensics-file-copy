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

#define BYTE_SIZE 8
#define FULL_BYTE 0xFF

INT4 inodeFromFilepath(int fd);
struct free_inode {
	UINT4 block_num;
	UINT4 inode_num;
}
struct free_inode claimFreeInode(); // return block number and inode number of free inode
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
    
    struct free_inode new_inode = claimFreeInode();
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

// Claims an inode and returns the block number and inode number
struct free_inode claimFreeInode() {
	UINT4 blockGroupNbr = 0;
	// Loop through block groups until a free inode is found
	while( true )
	{
		INT4 retVal;
		UINT1 i;
        	UINT1 b[BLK_SIZE];
	        // Use group 3's function to get block group at blockGroupNbr
		// UINT4 inodeBitmapBlock = Use group 3's function to get inode bitmap block
		// UINT4 inodeTableBlock = Use group 3's function to get inode table block
		// UINT4 inodesPerBlockGroup = Use superblock;
		// fseek64 to inodeBitmapBlock;

		// Read inode bitmap into memory
        	retVal = read(fd, b, BLK_SIZE);
        	if ( retVal <= 0 )
        	{
                	fprintf(stderr, "unable to read disk, retVal = %d\n", retVal );
                	exit(1);
        	}
		// This might need to be i < (inodesPerBlockGroup / BYTE_SIZE)?
		for (i = 0; i < BLK_SIZE; i++)
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
						fwrite(&claimed, 1, 1, fd);
						
						struct free_inode inode;
						inode.inode_num = (blockGroupNbr * inodesPerBlockGroup) + ((i * BYTE_SIZE) + j);
						inode.block_num = (((i * BYTE_SIZE) + j) * BLK_SIZE) + inodeTableBlock;
						return inode;
					}
				}
			}
		}
		blockGroupNbr++;
	}
	return NULL;
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






