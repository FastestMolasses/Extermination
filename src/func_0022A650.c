// NEARMISS func_0022A650  (vram 0x0022A650, 0x848 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc233 -O4,p -sdatathreshold 0 (991202 only 87.51). 524/530 instructions identical; 6 residuals in 3 classes, none fixable from source: (1) 1 instr — jump-table %lo addend: `addiu v1,v1,40` (jtbl_00273E60 at offset 40 in the concatenated build/jtblrodata .rodata) vs mwcc's per-table anonymous lo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: sibling of func_00200A40 — per-frame state machine for the task
// record pointed at by the scratchpad word at 0x70003B6C (SLOT).
//   SLOT[0xC]  = state, dispatched through jtbl_00273E60 (13 entries, 0..12)
//   SLOT[0xD]  = sub-step byte, cleared on every state change
//   SLOT[0x12] = cleared on entry to state 0
//   SLOT[0x13] = pending state, restored by state 10
//   unsigned short SLOT[0x1C] = cursor index into the int table D_002672E0[],
//                               wrapped to 0..8 (forward) / reset to 8 (backward)
// D_00810E78 = cursor-move mask (0x4000 next, 0x1000 previous);
// D_00810E74 = button mask (0x40 confirm, 0x20/0x10/0x100 cancel-ish).
// D_002672E0[SLOT[0x1C]] selects the action, dispatched through jtbl_00273E30
// (10 entries). The inner switch's cases are written in the order 7,0,1,2,3,4,9,
// 5,8,6 because mwcc lays case bodies out in source order and that is the order
// the target uses. Returns `hit` (0/1), or 1/2/3 from states 12/3/4.
// The SLOT fields are declared volatile: the target re-loads them on every read.

#define SLOT (*(unsigned char **)0x70003B6C)

extern void func_001AEE10();
extern void func_001AF6F0();
extern void func_001D2830();
extern void func_00200970();
extern int func_00201720();
extern int func_00201C50();
extern int func_00201F70();
extern int func_00202BA0();
extern int func_00202D10();
extern void func_0020A7A0();
extern void func_0020CD40();
extern void func_0020CD60();
extern void func_0020CDA0();
extern int func_00225AC0();
extern int func_0022A590();
extern void func_0022AEA0();
extern int func_0022B420();

extern int D_002672C0[];
extern int D_002672E0[];
extern int D_002821B0[];
extern int D_002821B4[];
extern int D_00282240[];
extern unsigned char D_00810118[];
extern unsigned short D_00810E6A[];
extern unsigned short D_00810E74[];
extern unsigned short D_00810E78[];

int func_0022A650(void)
{
    unsigned char *w;
    int hit;
    int r;

    w = D_00810118;
    hit = 0;
    func_001D2830(3, 1);
    switch (*(volatile unsigned char *)(SLOT + 0xC)) {
    case 0:
        D_002821B4[0] = 0;
        D_002821B0[0] = 4;
        D_00282240[0] = 8;
        *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0xC) + 1;
        *(volatile unsigned short *)(SLOT + 0x1C) = 0;
        *(volatile unsigned char *)(SLOT + 0x12) = 0;
        /* fallthrough */
    case 1:
        if (D_00810E6A[0] != 7) {
            w[1] = 0;
        }
        func_0020A7A0(0x20045EE59D421E40);
        if (D_00810E78[0] & 0x4000) {
            func_0020CDA0();
            *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
            if (*(volatile unsigned short *)(SLOT + 0x1C) >= 9) {
                *(volatile unsigned short *)(SLOT + 0x1C) = 0;
            }
            if (D_00810E6A[0] != 7) {
                if (D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                    *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
                    if (*(volatile unsigned short *)(SLOT + 0x1C) >= 9) {
                        *(volatile unsigned short *)(SLOT + 0x1C) = 0;
                    }
                }
            }
        } else if (D_00810E78[0] & 0x1000) {
            func_0020CDA0();
            *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) - 1;
            if (*(volatile signed char *)(SLOT + 0x1C) < 0) {
                *(volatile unsigned short *)(SLOT + 0x1C) = 8;
            }
            if (D_00810E6A[0] != 7) {
                if (D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                    *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) - 1;
                    if (*(volatile signed char *)(SLOT + 0x1C) < 0) {
                        *(volatile unsigned short *)(SLOT + 0x1C) = 8;
                    }
                }
            }
        } else if (D_00810E6A[0] != 7) {
            if (D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)] == 0) {
                *(volatile unsigned short *)(SLOT + 0x1C) = *(volatile unsigned short *)(SLOT + 0x1C) + 1;
            }
        }
        func_0022AEA0(w, D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)], 0);
        if (D_00810E74[0] & 0x40) {
            func_0020CD40();
            switch (D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)]) {
            case 7:
                hit = 1;
                break;
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
                *(volatile unsigned char *)(SLOT + 0x13) = 7;
                *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 3:
                *(volatile unsigned char *)(SLOT + 0x13) = 8;
                *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 4:
                *(volatile unsigned char *)(SLOT + 0x13) = 9;
                *(volatile unsigned char *)(SLOT + 0xC) = 0xA;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 9:
                func_001AF6F0();
                *(volatile unsigned char *)(SLOT + 0xC) = 3;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 5:
                *(volatile unsigned char *)(SLOT + 0xC) = 6;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                break;
            case 8:
                *(volatile unsigned char *)(SLOT + 0xC) = 4;
                *(volatile unsigned char *)(SLOT + 0xD) = 0;
                *(volatile unsigned char *)(SLOT + 0x13) = 0;
                break;
            case 6:
                break;
            }
        }
        if (hit == 1 || (D_00810E74[0] & 0x20) || (D_00810E74[0] & 0x10) ||
            (D_00810E74[0] & 0x100)) {
            func_0020CD60();
            *(volatile unsigned char *)(SLOT + 0xC) = 0xC;
        }
        break;
    case 2:
        func_00200970(1);
        *(volatile unsigned char *)(SLOT + 0xC) = 1;
        break;
    case 3:
        r = func_00225AC0(0);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 2;
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
            func_001AEE10(8, 0);
        } else if (r == 2) {
            return 2;
        }
        break;
    case 4:
        func_0020A7A0(0x20045EE59D421E40);
        func_0022AEA0(w, D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)], 1);
        r = func_0022B420(w, 0x86, D_002672C0, D_002672E0);
        if (r != 0) {
            if (r == 3) {
                return r;
            }
            if (r == 1) {
                *(volatile unsigned char *)(SLOT + 0xC) = 1;
            } else {
                *(volatile unsigned char *)(SLOT + 0xC) = 0xC;
            }
        }
        break;
    case 5:
        func_0020A7A0(0x20045EE59D421E40);
        func_0022AEA0(w, D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)], 1);
        r = func_00201720(w, 0x9E, D_002672C0, D_002672E0);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 1;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xC;
        }
        break;
    case 6:
        func_0020A7A0(0x20045EE59D421E40);
        func_0022AEA0(w, D_002672E0[*(volatile unsigned short *)(SLOT + 0x1C)], 1);
        r = func_00201C50(w, 0xB6, D_002672C0, D_002672E0);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 1;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xC;
        }
        break;
    case 7:
        r = func_00201F70(w, 0x86);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 2;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xB;
        }
        break;
    case 8:
        r = func_00202BA0();
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 2;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xB;
        }
        break;
    case 9:
        r = func_00202D10(w);
        if (r == 1) {
            *(volatile unsigned char *)(SLOT + 0xC) = 2;
        } else if (r == 2) {
            *(volatile unsigned char *)(SLOT + 0xC) = 0xB;
        }
        break;
    case 10:
        if (func_0022A590() != 0) {
            *(volatile unsigned char *)(SLOT + 0xC) = *(volatile unsigned char *)(SLOT + 0x13);
            *(volatile unsigned char *)(SLOT + 0xD) = 0;
        }
        break;
    case 11:
        func_00200970(1);
        *(volatile unsigned char *)(SLOT + 0xC) = 0xC;
        break;
    case 12:
        return 1;
    }
    return hit;
}
