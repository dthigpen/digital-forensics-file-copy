
struct StructuredStorageHeader {
    UINT1 _abSig[8]; // [offset from start in bytes, length in bytes]
    // [000H,08] {0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1} for current version,
    // was {0x0e, 0x11, 0xfc, 0x0d, 0xd0, 0xcf, 0x11, 0xe0} on old, beta 2 files (late ’92)
    UINT1 _clid[16]; // [008H,16] class id (set with WriteClassStg, retrieved with GetClassFile/ReadClassStg)
    UINT2 _uMinorVersion; // [018H,02] minor version of the format: 33 is written by reference implementation
    UINT2 _uDllVersion; // [01AH,02] major version of the dll/format: 3 is written by reference implementation
    UINT2 _uByteOrder; // [01CH,02] 0xFFFE: indicates Intel byte-ordering
    UINT2 _uSectorShift; // [01EH,02] size of sectors in power-of-two (typically 9, indicating 512-byte sectors)
    UINT2 _uMiniSectorShift; // [020H,02] size of mini-sectors in power-of-two (typically 6, indicating 64-byte mini-sectors)
    UINT2 _usReserved; // [022H,02] reserved, must be zero
    UINT4 _ulReserved1; // [024H,04] reserved, must be zero
    UINT4 _ulReserved2; // [028H,04] reserved, must be zero
    UINT4 _csectFat; // [02CH,04] number of SECTs in the FAT chain
    UINT4 _sectDirStart; // [030H,04] first SECT in the Directory chain
    UINT4 _signature; // [034H,04] signature used for transactionin: must be zero. 
    UINT4 _ulMiniSectorCutoff; // [038H,04] maximum size for mini-streams: typically 4096 bytes
    UINT4 _sectMiniFatStart; // [03CH,04] first SECT in the mini-FAT chain
    UINT4 _csectMiniFat; // [040H,04] number of SECTs in the mini-FAT chain
    UINT4 _sectDifStart; // [044H,04] first SECT in the DIF chain
    UINT4 _csectDif; // [048H,04] number of SECTs in the DIF chain
    UINT4 _sectFat[109]; // [04CH,436] the SECTs of the first 109 FAT sectors
};