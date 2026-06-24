// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Actor sound/effect dispatcher. Reads the actor's state byte at +0x14; when
// state==1 and the sub-actor selector byte (*(arg0 + arg0[0xA] + 0x6C))==2 it
// fires effect variant 0x25, otherwise variant 0x13 (state==1) or 0x19 (other)
// via func_001FCBD0(0x3C, 0x9B, 7, variant, 0x70808080) (the 0x70808080 5th
// arg is a packed color/flag passed in $t0). Then dispatches on the global
// input/event flags at D_00810E74: bit 0x40 -> func_0020CD40, return 1;
// bit 0x20 -> func_0020CD60, return 1; bit 0x10 -> func_0020CD60, return 2;
// else return 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the tail collapses to a beqzl (branch-likely) and drops CW's dead
// trailing return-block recompute (wall: mwcc-vs-CW branch lowering), capping
// it at 91.0%. The 2.3.3 build reproduces CW's plain-beqz + dead-block shape,
// so this readable C is byte-identical. Verified objdiff 100.0 vs
// build/expected/func_00227160.o.
extern int func_001FCBD0(int, int, int, int, unsigned int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern unsigned short D_00810E74;

int func_00227160(unsigned char *arg0) {
    unsigned char a1;

    a1 = arg0[0x14];
    if (a1 == 1 && arg0[arg0[0xA] + 0x6C] == 2) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x25, 0x70808080);
    } else if (a1 == 1) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x13, 0x70808080);
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 0x19, 0x70808080);
    }
    if (D_00810E74 & 0x40) {
        func_0020CD40();
        return 1;
    }
    if (D_00810E74 & 0x20) {
        func_0020CD60();
        return 1;
    }
    if (D_00810E74 & 0x10) {
        func_0020CD60();
        return 2;
    }
    return 0;
}
