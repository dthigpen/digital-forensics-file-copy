#include "recoverdocinc.h"
#include "inodeinc.h"

VOID main(INT4 argc, CHAR** argv )
{
    INT4 fd;
    UINT1 u1SearchFlags = SCAN_FREE_BLOCKS;

    if (argc < 2)
    {
        printf("Example usage: sudo ./recoverdoc <partition> <optional-searchflag>\n");
        printf("\t <searchflag> values: 0 (search free data blocks) (default)\n");
        printf("\t              values: 1 (search used data blocks)\n");
        printf("\t              values: 2 (search all data blocks)\n");
        return;
    } else if (argc > 2) {
    u1SearchFlags = atoi(argv[2]);
    }  
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
        perror("Error opening file");
        return;
    }
    InodeInit(fd);
    //FindIndirectBlocks(u1SearchFlags);
    // RecoverDocFindSignatures(u1SearchFlags);
    RecoverDocFindMatches(u1SearchFlags);
    InodeInitExit();
}
