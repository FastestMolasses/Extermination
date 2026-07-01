// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Enemy/entity per-frame update dispatched on the mode byte at arg0+0x14.
// Depending on mode (1 / 0x68!=0 / else) it kicks a HUD/particle spawn via
// func_001FCBD0(0x3C,0x9B,7,<variant>,0x70808080). Then advances a
// pathing/steering step with func_001FE9A0(1, arg0+0x50, arg0+0x54); if that
// fails (0) the whole update aborts returning 0. Global damage/effect flags
// D_00810E74 gate three early exits (bits 0x20->2, 0x10->3, 0x40->1 with a
// mode-2 sub-check on the byte at arg0[arg0[0xA]+0x6C]), each running
// func_0020CD40/60 + func_001FE8D0. Bits in D_00810E78 tune the counter at
// arg0+0xA (0x1000 decrements while >0; 0x4000 increments while <4, both with
// func_0020CDA0). Finally runs the motion solve func_001FECB0(arg0+0x48,
// arg0+0x4C, arg0+0x58, 0, 0); result classification: while global D_00275C58
// is set -> 4; result 0 -> (arg0+0x58==2 ? 0 : 4); result -1 -> 0; else 4.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 92.3%. Keys: (1) -sdatathreshold 4 makes the int global D_00275C58 gp-relative
// while the 16-bit flag words D_00810E74/E78 are declared as arrays (>4 bytes) so
// they stay absolute; (2) the arg0[arg0[0xA]+0x6C] address is built as an int
// (byteval + (int)arg0) so mwcc emits `addu v1,v1,s0` in the target's operand
// order rather than `addu v1,s0,v1`.
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, void *, void *);
extern int func_001FECB0(int, int, void *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern void func_0020CDA0(void);
extern int D_00275C58;
extern unsigned short D_00810E74[4];
extern unsigned short D_00810E78[4];
#define E74 (D_00810E74[0])
#define E78 (D_00810E78[0])

int func_00229170(char *arg0) {
    int r;

    if (*(unsigned char *)(arg0 + 0x14) == 1) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x11, 0x70808080);
    } else if (*(int *)(arg0 + 0x68) != 0) {
        func_001FCBD0(0x3C, 0x9B, 7, 0x17, 0x70808080);
    } else {
        func_001FCBD0(0x3C, 0x9B, 7, 6, 0x70808080);
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    if (E74 & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }
    if (E74 & 0x10) {
        func_0020CD60();
        func_001FE8D0();
        return 3;
    }
    if (E74 & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        if (*(unsigned char *)(arg0 + 0x14) == 2) {
            int i = *(unsigned char *)(arg0 + 0xA) + (int)arg0;
            if (*(unsigned char *)(i + 0x6C) == 1) {
                return 1;
            }
        } else {
            return 1;
        }
    }
    if ((E78 & 0x1000) && *(unsigned char *)(arg0 + 0xA) > 0) {
        func_0020CDA0();
        *(unsigned char *)(arg0 + 0xA) = *(unsigned char *)(arg0 + 0xA) - 1;
    }
    if (E78 & 0x4000) {
        if (*(unsigned char *)(arg0 + 0xA) < 4) {
            *(unsigned char *)(arg0 + 0xA) = *(unsigned char *)(arg0 + 0xA) + 1;
            func_0020CDA0();
        }
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
