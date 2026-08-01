// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: one-time CD/DVD lookup of the two streamed-audio containers under
// \O_STREAM (the symbol name comes from the "\O_STREAM\MUSIC.DAT;1" literal).
//
// For each file it busy-waits on the libcdvd pair
//   func_00113280 = sceCdInit(mode)  /  func_00111C28 = sceCdSearchFile(fp, name)
// re-initialising the drive on every failed lookup until the search succeeds,
// then caches the file's start LSN:
//   D_00282188 = LSN of the file named by D_0026EBB0
//   D_0028218C = LSN of the file named by D_0026EBD0
// Both LSNs are read out of the same 0x20-byte sceCdlFILE on the stack.
//
// Note the first lookup is preceded by a bare sceCdInit(0) (drive warm-up)
// while the second reuses the drive state left by the first loop, which is why
// the two loops are not symmetric.
extern int func_00113280(int mode);
extern int func_00111C28(void *fp, const char *name);

extern char D_0026EBB0[];
extern char D_0026EBD0[];
extern int D_00282188;
extern int D_0028218C;

typedef struct {
    unsigned int lsn;
    unsigned int size;
    char name[16];
    unsigned char date[8];
} CdFile;

void sub_O_STREAM_MUSIC_DAT_1(void)
{
    CdFile fp;

    func_00113280(0);
    while (func_00111C28(&fp, D_0026EBB0) == 0) {
        func_00113280(0);
    }
    D_00282188 = fp.lsn;
    while (func_00111C28(&fp, D_0026EBD0) == 0) {
        func_00113280(0);
    }
    D_0028218C = fp.lsn;
}
