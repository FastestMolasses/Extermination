// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2
//
// Actor animation-instance init. Binds the actor (arg0) to an animation
// resource: func_001CA6E0(actor, func_001C6120(D_0028A56C)). If arg2 != -1,
// sets actor+0x40 from the global anim table D_0028A490[arg2]. Stores the
// bone count func_001C6150(actor+0x44) into actor+0xC; if it exceeds the cap
// D_00275BCC, sets actor+0x4 = 3 (error state) and returns 1. Otherwise
// allocates one matrix per bone via func_001AF780() into actor+0x110[i],
// records the count at actor+0x9, runs anim_bone_array_setup(count), ret 0.
// (arg1 is unused in this variant; only func_001B10B0 reads it.)
//
// -sdatathreshold 2: the s16 cap D_00275BCC is gp-relative (lh gp), while the
// 4-byte D_0028A56C / D_0028A490 stay hi/lo. The loop is a comma-init for()
// so the counter init (s1=0) fills the beqz delay slot and the pointer init
// (p=arg0) fills the b delay slot, and the duplicated s1=0 preheader matches
// CW. Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): 991202 reaches only 92.41%
// (wall #13). Verified objdiff 100% vs func_001B0DC0.o.
extern int func_001AF780(void);
extern int func_001C6120(int);
extern unsigned char func_001C6150(int);
extern int func_001CA6E0(char *, int);
extern void anim_bone_array_setup(unsigned char);
extern short D_00275BCC;
extern int D_0028A490[];
extern int D_0028A56C;

int func_001B0DC0(char *arg0, int arg1, int arg2) {
    int i;
    char *p;
    unsigned char n;

    func_001CA6E0(arg0, func_001C6120(D_0028A56C));
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
