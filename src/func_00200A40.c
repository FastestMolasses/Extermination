// NEARMISS func_00200A40  (vram 0x00200A40, 0x848 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 -O4,p -sdatathreshold 4 (991202 only 89.76; threshold 4 needed for the gp-rel `unsigned char D_00275BD8`, threshold 0 costs 0.5%). 525/530 instructions identical; 5 residuals in the SAME 3 classes as func_0022A650: (1) 1 instr — jump-table %lo addend `addiu v1,v1,28` (jtbl_002732F0 at off...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame state machine for the task record pointed at by the
// scratchpad word at 0x70003B6C (SLOT).
//   SLOT[0xC]  = state, dispatched through jtbl_002732F0 (11 entries, 0..10)
//   SLOT[0xD]  = sub-step byte, cleared on every state change
//   SLOT[0x12] = cleared on entry to state 3
//   unsigned short SLOT[0x1C] = cursor index into the int table D_00264EC0[],
//                               wrapped to 0..6 (forward) / reset to 6 (backward)
// D_00810E78 is the "cursor move" bitmask (0x4000 = next, 0x1000 = previous),
// D_00810E74 the "button" bitmask (0x40 = confirm, 0x20/0x10 = cancel-ish).
// D_00264EC0[SLOT[0x1C]] selects the action, dispatched through jtbl_002732D0
// (7 entries). Returns 1 only from state 10 when D_0028A9A0 == 2, else 0.
// The SLOT fields are declared volatile: the target re-loads them on every read
// instead of CSE-ing, which is what volatile reproduces.

#define SLOT (*(unsigned char **)0x70003B6C)

extern void func_001AEDE0();
extern void func_001AEE10();
extern void func_001D2830();
extern void func_001FABB0();
extern void func_001FBC50();
extern void func_001FF080();
extern void func_00201290();
extern int func_00201720();
extern int func_00201C50();
extern int func_00201F70();
extern int func_00202BA0();
extern int func_00202D10();
extern void func_0020A7A0();
extern void func_0020CD40();
extern void func_0020CD60();
extern void func_0020CDA0();

extern int D_00264EA0[];
extern int D_00264EC0[];
extern unsigned char D_00275BD8;
extern char D_00282157[];
extern int D_002821B0[];
extern int D_002821B4[];
extern int D_00282240[];
extern short D_0028A9A0[];
extern unsigned char D_00810118[];
extern unsigned short D_00810E6A[];
extern unsigned short D_00810E74[];
extern unsigned short D_00810E78[];

int func_00200A40(void)
{
    unsigned char *w;
    int hit;
    int r;

    w = D_00810118;
    hit = 0;
    func_001D2830(3, 1);
    switch (*(volatile unsigned char *)(SLOT + 0xC)) {
    case 0:
        func_001FBC50();
        func_001FABB0();
        *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) + 1;
        break;
    case 1:
        if (D_00282157[0] == 0) {
            *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) + 1;
            D_00275BD8 = 1;
            func_001FF080(0, 0x2B);
        }
        break;
    case 2:
        if (D_00275BD8 == 0) {
            *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) + 1;
        }
        break;
    case 3:
        D_002821B4[0] = 0;
        D_002821B0[0] = 4;
        D_00282240[0] = 8;
        *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) + 1;
        *(volatile unsigned short *)(SLOT + 0x1C) = 0;
        *(volatile unsigned char *)(SLOT + 0x12) = 0;
        func_001AEE10(8, 0);
        /* fallthrough */
    case 4:
        if (D_00810E6A[0] != 7) {
            w[1] = 0;
        }
        func_0020A7A0(0x200435259D422180);
        if (D_00810E78[0] & 0x4000) {
            func_0020CDA0();
            *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
            if (*(volatile unsigned short *)(SLOT + 0x1C) >= 7) {
                *(volatile unsigned short *)(SLOT + 0x1C) = 0;
            }
            if (D_00810E6A[0] != 7) {
                if (D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                    *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
                    if (*(volatile unsigned short *)(SLOT + 0x1C) >= 7) {
                        *(volatile unsigned short *)(SLOT + 0x1C) = 0;
                    }
                }
            }
        } else if (D_00810E78[0] & 0x1000) {
            func_0020CDA0();
            *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) - 1;
            if (*(volatile signed char *)(SLOT + 0x1C) < 0) {
                *(volatile unsigned short *)(SLOT + 0x1C) = 6;
            }
            if (D_00810E6A[0] != 7) {
                if (D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                    *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) - 1;
                    if (*(volatile signed char *)(SLOT + 0x1C) < 0) {
                        *(volatile unsigned short *)(SLOT + 0x1C) = 6;
                    }
                }
            }
        } else if (D_00810E6A[0] != 7) {
            if (D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
            }
        }
        func_00201290(w, D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)], 0);
        if (D_00810E74[0] & 0x40) {
            func_0020CD40();
            switch (D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)]) {
            case 0:
                if (D_00810E6A[0] != 7) {
                    break;
                }
                /* fallthrough */
            case 1:
                *(volatile unsigned char *)(SLOT + 0xC) = 5;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 2:
                *(volatile unsigned char *)(SLOT + 0xC) = 7;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 3:
                *(volatile unsigned char *)(SLOT + 0xC) = 8;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 4:
                *(volatile unsigned char *)(SLOT + 0xC) = 9;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 5:
                *(volatile unsigned char *)(SLOT + 0xC) = 6;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 6:
                hit = 1;
                break;
            }
        }
        if (hit == 1 || (D_00810E74[0] & 0x20) || (D_00810E74[0] & 0x10)) {
            func_0020CD60();
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 5:
        func_0020A7A0(0x200435259D422180);
        func_00201290(w, D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)], 1);
        r = func_00201720(w, 0x86, D_00264EA0, D_00264EC0);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) - 1;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 6:
        func_0020A7A0(0x200435259D422180);
        func_00201290(w, D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)], 1);
        r = func_00201C50(w, 0x86, D_00264EA0, D_00264EC0);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 4;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 7:
        r = func_00201F70(w, 0x86);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 4;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 8:
        r = func_00202BA0();
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 4;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 9:
        r = func_00202D10(w);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 4;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEDE0(8, 0);
        }
        break;
    case 10:
        if (D_00810E6A[0] != 7) {
            w[1] = 0;
        }
        func_0020A7A0(0x200435259D422180);
        if (D_00810E6A[0] != 7) {
            if (D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
            }
        }
        func_00201290(w, D_00264EC0[*(volatile unsigned short *)(SLOT + 0x1C)], 0);
        if (D_0028A9A0[0] == 2) {
            return 1;
        }
        break;
    }
    return 0;
}
