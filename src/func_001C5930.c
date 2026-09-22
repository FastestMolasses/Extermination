// NEARMISS func_001C5930  (vram 0x001C5930, 0x318 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Semantic corrections were checked against original instructions; remaining code still needs audit.
// Remaining differences in this candidate:
// Semantic correction against original branches; canonical score remeasured. Assembly remains linked unless full compiled-C gate proves exactness.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern volatile unsigned char D_70003B8D;           /* PS2 scratchpad @ 0x70003B8D */

extern void func_001AFC10(unsigned char *);
extern int func_001C5860(void);
extern int func_001CC170(int);
extern int func_001CC1E0(int, int, int, int, int, int, int *);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_008106B8;
extern short D_00289B40[][2];
extern int *D_002671C0[];
extern int *D_0026726C[];

void func_001C5930(unsigned char *arg0) {
    unsigned char *sub;
    unsigned char st;
    int area;
    int v0;
    int idx;
    int *strptr;
    int w;
    int cx;
    int flag;
    short v1;
    unsigned char spad;

    st = *(unsigned char *)(arg0 + 4);
    sub = arg0 + 0x1F0;
    switch (st) {
    case 0:
        *(short *)(arg0 + 0x28) = 0x12C;
        area = D_00810700;
        *(short *)(arg0 + 0x2A) = D_00289B40[area][0];
        v1 = *(short *)(arg0 + 0x2A);
        *(short *)(arg0 + 0x2A) = v1 + D_00810701;
        *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        v0 = func_001C5860();
        *(int *)sub = v0;
        *(short *)(sub + 4) = 0x12C;
        break;
    case 1:
        spad = D_70003B8D;
        /* 0x001C59E8..0x001C5A10: the branch-delay assignment clears
         * the suppress flag for selectors outside 1..3.  The earlier
         * readable reconstruction reversed this test for 0 and 2/3. */
        flag = 0;
        if (spad == 1 || (unsigned)(spad - 2) < 2) {
            flag = 1;
        }
        st = *(unsigned char *)(arg0 + 5);
        if (st == 1) {
            goto L_5AA8;
        }
        if (st == 0) {
            if (!flag) {
                idx = *(short *)(arg0 + 0x2A);
                strptr = D_002671C0[idx];
                w = func_001CC170((int)strptr);
                cx = 0x800 - (w >> 1);
                func_001CC1E0(1, cx, 0x7A2, 0xA, 0x14, (int)strptr, 0);
            }
            *(short *)(arg0 + 0x28) = *(short *)(arg0 + 0x28) - 1;
            if (*(short *)(arg0 + 0x28) != 0) {
                goto L_5AA8;
            }
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        } else {
            goto L_5AC0;
        }
    L_5AA8:
        if (D_008106B8 != 0) {
            *(unsigned char *)(arg0 + 4) = 3;
        }
    L_5AC0:
        if (flag) {
            break;
        }
        st = *(unsigned char *)(arg0 + 6);
        if (st == 1) {
            v0 = func_001C5860();
            v1 = *(int *)sub;
            if (v1 == v0) {
                break;
            }
            *(int *)sub = v0;
            *(unsigned char *)(arg0 + 6) = 0;
            break;
        }
        if (st == 0) {
            area = *(int *)sub;
            if (area != 0) {
                if (*(unsigned char *)(arg0 + 5) != 1) {
                    break;
                }
                strptr = D_0026726C[area];
                if (area != 5) {
                    w = func_001CC170((int)strptr);
                    cx = 0x896 - (w >> 1);
                    func_001CC1E0(1, cx, 0x7A2, 0xA, 0x14, (int)strptr, 0);
                    v1 = *(short *)(sub + 4);
                } else {
                    w = func_001CC170((int)strptr);
                    cx = 0x896 - (w >> 1);
                    func_001CC1E0(1, cx, 0x7A2, 0xA, 0x14, (int)strptr, 0);
                    v1 = *(short *)(sub + 4);
                }
                v1 = v1 - 1;
                *(short *)(sub + 4) = v1;
                if (v1 != 0) {
                    v0 = func_001C5860();
                    v1 = *(int *)sub;
                    if (v1 == v0) {
                        break;
                    }
                    *(int *)sub = v0;
                    *(short *)(sub + 4) = 0x12C;
                    break;
                }
                *(short *)(sub + 4) = 0x12C;
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                break;
            }
            *(short *)(sub + 4) = 0x12C;
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        }
        break;
    case 2:
        func_001AFC10(arg0);
        break;
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
