// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame state driver for an entity at arg0. First, depending on phase byte
// arg0+0x14 and the sub-state byte at arg0[arg0+0xA]+0x6C, it kicks one of three
// effects via func_001FCBD0(0x3C, 0x9B, 7, sel, 0x70808080) -- sel = 0x24 when
// (phase==1 && substate==2), 0x12 when phase==1 otherwise, 0x18 when phase!=1.
// func_001FCBD0 takes a hidden 5th argument in $t0 (the 0x70808080 GIF/VIF addr).
// Then it advances the timer state at arg0+3: state 0 arms the timer (arg0+3=1,
// arg0+0x10=0x2D) and returns 0; state 1 decrements arg0+0x10 and, once it hits
// 0, if (phase==1 && substate==2) clears arg0+1 and calls func_00225CF0(self,2,7)
// returning 0, else returns 3 (timer-not-yet-expired and other states return 0).
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202 reaches 90.8.
// Keys: (1) func_001FCBD0's real arity is 5 -- the trailing 0x70808080 emits the
// `lui v0,0x7080 / ori t0,v0,0x8080` pair the target sets up before each call;
// (2) the arg0[0xA]+arg0+0x6C index must be byte-first to reproduce `addu v1,v1,s0`
// -- written as ((int)byte + (int)arg0 + 0x6C); (3) an ASCENDING switch(st) plus a
// single shared `return 0` tail reproduces CW's merged return-0 epilogue, and the
// explicit `return 0` right after func_00225CF0 lets mwcc drop `paddub v0,zero,zero`
// into that call-path branch's delay slot (the target's clean-store-style slot fill).
extern void func_001FCBD0(int, int, int, int, int);
extern void func_00225CF0(unsigned char *, int, int);

int func_00227010(unsigned char *arg0) {
    unsigned char st;
    unsigned char ph;

    ph = *(unsigned char *)(arg0 + 0x14);
    if (ph == 1 &&
        *(unsigned char *)((int)*(unsigned char *)(arg0 + 0xA) + (int)arg0 + 0x6C) == 2) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x24, 0x70808080);
    } else if (ph == 1) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x12, 0x70808080);
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 0x18, 0x70808080);
    }

    st = *(unsigned char *)(arg0 + 3);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 3) = 1;
        *(int *)(arg0 + 0x10) = 0x2D;
        break;
    case 1:
        *(int *)(arg0 + 0x10) = *(int *)(arg0 + 0x10) - 1;
        if (*(int *)(arg0 + 0x10) == 0) {
            if (*(unsigned char *)(arg0 + 0x14) == 1 &&
                *(unsigned char *)((int)*(unsigned char *)(arg0 + 0xA) + (int)arg0 + 0x6C) == 2) {
                *(unsigned char *)(arg0 + 1) = 0;
                func_00225CF0(arg0, 2, 7);
                return 0;
            } else {
                return 3;
            }
        }
        break;
    }
    return 0;
}
