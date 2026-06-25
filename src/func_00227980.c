// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Actor per-frame state-machine update (object pointer in arg0). Selects a
// sound-effect variant by the actor's input/sub-actor state (+0x14, and the
// selector byte arg0[arg0[0xA]+0x6C]) via func_001FCBD0(...,0x70808080) (the
// 0x70808080 5th arg is a packed color/flag in $t0), then gates on
// func_001FE9A0(1,&+0x50,&+0x54). Dispatches the +0x3 state byte: state 0 inits
// to state 1; state 1 handles the D_00810E74 flag bits (0x40 abort->3/1,
// 0x20->1, 0x10->2, 0x2000/0x8000 toggles). Finally drives func_001FECB0(+0x48,
// +0x4C,&+0x58,0,0): if the busy flag D_00275C58 is set return 4, else map the
// step result (r==0 -> *(0x58)==2?0:4, r==-1 -> 0, else 4).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Three
// idioms are load-bearing: (1) the func_001FECB0 tail must be r==0 fall-through
// first with the r==-1 guard as `if(r==-1)return 0;` to match CW's branch
// lowering (991202 caps ~91.4%); (2) D_00810E74 is over-declared `unsigned
// short[8]` and accessed [0] to force absolute addressing while D_00275C58
// stays gp-rel at -sdatathreshold 8 (idiom #20); (3) arg0 is `unsigned char*`
// with the selector written as the array index arg0[arg0[0xA]+0x6C] so mwcc
// emits `addu v1,v1,s0` (value+base) matching the target. Verified objdiff
// 100.0 vs build/expected/func_00227980.o.
extern int func_001FCBD0(int, int, int, int, unsigned int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, unsigned char *, unsigned char *);
extern int func_001FECB0(int, int, unsigned char *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern void func_002256E0(void);
extern void func_00225700(void);
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_00227980(unsigned char *arg0) {
    unsigned char v1;
    int r;

    if (arg0[0x14] == 1) {
        v1 = arg0[arg0[0xA] + 0x6C];
        switch (v1) {
        case 0:
            func_001FCBD0(0x3C, 0x9B, 7, 0x14, 0x70808080);
            break;
        case 1:
            func_001FCBD0(0x3C, 0x9B, 7, 0x15, 0x70808080);
            break;
        case 2:
            func_001FCBD0(0x3C, 0x9B, 7, 0x27, 0x70808080);
            break;
        }
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 0x1A, 0x70808080);
    }
    func_001FCBD0(0x8C, 0xC3, 7, 2, 0x70808080);
    v1 = *(unsigned char *)(arg0 + 9);
    switch (v1) {
    case 0:
        func_00225700();
        break;
    case 1:
        func_002256E0();
        break;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    v1 = *(unsigned char *)(arg0 + 3);
    switch (v1) {
    case 0:
        *(unsigned char *)(arg0 + 3) = 1;
        *(unsigned char *)(arg0 + 9) = 0;
        break;
    case 1:
        if (D_00810E74[0] & 0x40) {
            func_0020CD40();
            func_001FE8D0();
            if (*(unsigned char *)(arg0 + 9) == 1) {
                *(unsigned char *)(arg0 + 9) = 0;
                return 3;
            }
            *(unsigned char *)(arg0 + 9) = 0;
            return 1;
        }
        if (D_00810E74[0] & 0x20) {
            func_0020CD60();
            *(unsigned char *)(arg0 + 9) = 0;
            func_001FE8D0();
            return 1;
        }
        if (D_00810E74[0] & 0x10) {
            func_0020CD60();
            *(unsigned char *)(arg0 + 9) = 0;
            func_001FE8D0();
            return 2;
        }
        if ((D_00810E74[0] & 0x2000) && *(unsigned char *)(arg0 + 9) != 0) {
            func_0020CDA0();
            *(unsigned char *)(arg0 + 9) = 0;
        }
        if ((D_00810E74[0] & 0x8000) && *(unsigned char *)(arg0 + 9) == 0) {
            func_0020CDA0();
            *(unsigned char *)(arg0 + 9) = 1;
        }
        break;
    }
    r = func_001FECB0(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), arg0 + 0x58, 0, 0);
    if (D_00275C58 != 0) {
        return 4;
    }
    if (r == 0) {
        if (*(int *)(arg0 + 0x58) != 2) {
            return 4;
        }
        return 0;
    }
    if (r == -1) {
        return 0;
    }
    return 4;
}
