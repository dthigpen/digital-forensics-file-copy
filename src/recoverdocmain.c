#include "recoverdocinc.h"
#include "inodeinc.h"

VOID main(INT4 argc, CHAR** argv )
{
    CHAR *Partition;
    CHAR *InodeString;
    CHAR *FileName;
    INT4 fd;
    UINT4 u4OldInodeNo;

    if (argc < 2)
    {
        printf("Example usage: sudo ./recoverdoc <partition>\n");
        return;
    } else if (argc >= 4) {
	Partition = argv[1];
    }
    printf("Args: partition: %s\n", argv[1]);
    
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("Error opening file");
        return;
    }
    InodeInit(fd);
    RecoverDocFindSignatures();
    InodeInitExit();
}