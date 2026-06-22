// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2
//
// Variant of func_001B0DC0: same actor animation-instance init, but binds the
// resource by table index instead of a fetched handle:
// func_001CA6E0(actor, D_0028A490[arg1]). If arg2 != -1, sets actor+0x40 from
// D_0028A490[arg2]. Stores bone count func_001C6150(actor+0x44) into
// actor+0xC; if it exceeds the cap D_00275BCC, sets actor+0x4 = 3 and returns
// 1. Otherwise allocates one matrix per bone via func_001AF780() into
// actor+0x110[i], records count at actor+0x9, runs anim_bone_array_setup,
// returns 0.
//
// Same matching idioms as func_001B0DC0 (-sdatathreshold 2 for the gp-rel s16
// cap; comma-init for() loop reproducing the dual delay-slot fills and the
// duplicated counter-init preheader). Built with mwcc 2.3.3
// (mwcps2-2.3.3-000906): 991202 reaches only 85.27% (wall #13). Verified
// objdiff 100% vs func_001B10B0.o.
extern int func_001AF780(void);
extern unsigned char func_001C6150(int);
extern int func_001CA6E0(char *, int);
extern void anim_bone_array_setup(unsigned char);
extern short D_00275BCC;
extern int D_0028A490[];

int func_001B10B0(char *arg0, int arg1, int arg2) {
    int i;
    char *p;
    unsigned char n;

    func_001CA6E0(arg0, D_0028A490[arg1]);
    if (arg2 != -1) {
        *(int *)(arg0 + 0x40) = D_0028A490[arg2];
    }
    *(unsigned char *)(arg0 + 0xC) = func_001C6150(*(int *)(arg0 + 0x44));
    if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
        *(char *)(arg0 + 4) = 3;
        return 1;
    }
    for (i = 0, p = arg0; i < (int)(n = *(unsigned char *)(arg0 + 0xC)); i += 1) {
        *(int *)(p + 0x110) = func_001AF780();
        p += 4;
    }
    *(unsigned char *)(arg0 + 9) = n;
    anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
    return 0;
}
