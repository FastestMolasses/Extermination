// NEARMISS func_002160B0  (vram 0x002160B0, 0xFD8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.22% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 99.216% (mwcc233 -O4,p -sdatathreshold 0); ~24 of 1014 instructions differ, in three clusters, all body-correct. (1) REGALLOC PERMUTATION in the second scan loop (13 instrs): identical instruction sequence, but the target colours {bound=a1, counter=a2, element/address=a0, p[0x1B]=v1} while mwcc p...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: sibling of func_002149F0 - the "transfer / gauge" page of the same
// system-menu framework. `p` is the menu-page object; p[5] is the sub-state,
// dispatched through a 7-entry jump table (jtbl_00273770). The case bodies are
// emitted 0,1,2,6,3,4,5, so the source case order must match. Same per-frame
// quartet (func_0020A7A0 / func_0020AE40 / func_0020B210 / func_0020B0D0).
//   0  build the entry list from the five availability flags D_00810C82..86 (kinds
//      0..4 appended at p[0x50+n]); reset D_002821B0/B4/40 and the base string id
//      p+0x1E = 0x1E. Then branch on the pending request D_008106B0: code 4 selects
//      the "kind 2" entry (page p[0x19] = (i>>2)*4, cursor p[0x17] = i%4) and goes
//      to state 4, unless the gauge is already full (D_0081085C == 0 and
//      D_00810858 >= 100), which routes to the state-6 banner with message 0x1A.
//      Any other code selects the entry matching D_008106B1-0x1E and goes to state 3.
//      With no pending request, advance to state 1 and fall through.
//   1  idle list page. Cancel (0x20) closes the page. Confirm (0x40) resolves the
//      highlighted entry kind: kind 2 needs a record (func_00185420) and a
//      non-full gauge; kinds 3/4 and the rest gate on D_00810707 (60 vs 100 cap)
//      and on D_00810858. Failures go to the state-6 banner with message
//      0x1A / 0x19 / 1 / 0x1B; success goes to state 4.
//   2  page-close animation; steps back to state 1 when func_0020BC50 finishes.
//   6  banner state (message id already in D_002821B8); 240 frames or a button.
//   3  short banner, then back to the list.
//   4  confirm dialog. The prompt id is 0x12 for kind 2, else 0x18. The cursor
//      moves through func_0020CDA0. On confirm, resolve the highlighted kind and
//      set the target gauge value p+0x34 and the spend p+0x38:
//        0  +30, clamped to the 60/100 cap, then state 5 (roll animation);
//        1  full 100, clamped to the cap, then state 5;
//        2  full 100, spend 0, mark the record (1/5) and hand it to func_0015C750;
//        3  +30 clamped; spend D_0081085C (minus 30, floored at 0, on normal);
//        4  cap value; spend D_0081085C or 0.
//      Then play the entry's string (func_001C47E0), refresh (func_00215FE0) and
//      clamp the cursor into the shortened list.
//   5  gauge roll animation: ticks D_00810858 up by 1 every 3 frames (with a click
//      every 10 global frames); on arrival - or on any button in 0x870 - commits
//      p+0x34 into D_00810858 / D_008104D0, releases through func_0015C700 and
//      returns to state 1.
// NOTE: case 4 reads p[0x50 + idx] where `idx` is the case-0 scan counter and is
// NOT initialised when state 4 is entered directly. That is faithful to the
// original (the compiler keeps it in $s0 across the switch); mwcc warns about it.

extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern unsigned char D_008102B0;
extern float D_008104D0;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern char *D_008106D0;
extern unsigned char D_00810707;
extern float D_00810858;
extern float D_0081085C;
extern unsigned char D_00810C82;
extern unsigned char D_00810C83;
extern unsigned char D_00810C84;
extern unsigned char D_00810C85;
extern unsigned char D_00810C86;
extern unsigned short D_00810E74;
extern char D_00265FF0[];
extern char D_00266060[];

extern void func_0015C700(void *s);
extern void func_0015C750(void *s, int a, int b, void *rec);
extern char *func_00185420(int id);
extern void func_001C47E0(int id, int a);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001FCF10();
extern void func_00207D00(int a, int b);
extern void func_0020A7A0(long long tag);
extern void func_0020AE40(void *p, void *q, int n);
extern void func_0020B0D0(void *p, void *q);
extern int func_0020B210(void *p, void *q, long long tag, int flags);
extern void func_0020BBE0(void *p, int n);
extern int func_0020BC50(void *p, void *q, long long tag, int flags);
extern int func_0020BEF0(void *p);
extern void func_0020CCB0(void *p);
extern void func_0020CD40();
extern void func_0020CD60();
extern void func_0020CD80();
extern void func_0020CDA0(void *p);
extern void func_00215FE0(void *p);

void func_002160B0(unsigned char *p) {
    char *rec;
    int idx;
    int n;
    int j;
    int c1;
    int c2;
    int kind;
    int ev;
    float lvl;

    switch (p[5]) {
    case 0:
        p[0x17] = 0;
        p[0x19] = 0;
        p[0x18] = 0;
        p[0x1A] = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 3;
        *(short *)(p + 0x1E) = 0x1E;
        if (D_00810C82 != 0) {
            p[0x50 + p[0x18]++] = 0;
        }
        if (D_00810C83 != 0) {
            p[0x50 + p[0x18]++] = 1;
        }
        if (D_00810C84 != 0) {
            p[0x50 + p[0x18]++] = 2;
        }
        if (D_00810C85 != 0) {
            p[0x50 + p[0x18]++] = 3;
        }
        if (D_00810C86 != 0) {
            p[0x50 + p[0x18]++] = 4;
        }
        if (D_008106B0 != 0) {
            if ((D_008106B0 & 0xFF) == 4) {
                D_008106B0 = 0;
                *(char **)(p + 0x30) = D_008106D0;
                if (D_0081085C == 0.0f && !(D_00810858 < 100.0f)) {
                    D_002821B4 = 0;
                    D_002821B8 = 0x1A;
                    p[5] = 6;
                    p[6] = 0xF0;
                    break;
                }
                c1 = p[0x18];
                idx = 0;
                goto scan1_test;
            scan1_body:
                if (p[0x50 + idx] == 2) {
                    p[0x19] = (idx >> 2) * 4;
                    p[0x17] = idx % 4;
                    goto scan1_done;
                }
                idx++;
            scan1_test:
                if (idx < c1) {
                    goto scan1_body;
                }
            scan1_done:
                p[5] = 4;
                p[6] = 1;
                break;
            }
            D_008106B0 = 0;
            p[0x1B] = D_008106B1 - 0x1E;
            c2 = p[0x18];
            j = 0;
            goto scan2_test;
        scan2_body:
            if (p[0x50 + j] == p[0x1B]) {
                p[0x19] = (j >> 2) * 4;
                p[0x17] = j % 4;
                D_00282240 = 4;
                goto scan2_done;
            }
            j++;
        scan2_test:
            if (j < c2) {
                goto scan2_body;
            }
        scan2_done:
            p[5] = 3;
            p[6] = 0xF0;
            break;
        }
        p[5] = p[5] + 1;
        /* fallthrough */
    case 1:
        if (D_00810E74 & 0x20) {
            func_0020CD60();
            D_002821B4 = 2;
            p[1] = 3;
            p[2] = 0;
            p[3] = 0;
            p[4] = 0;
            p[5] = 0;
            break;
        }
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        if (func_0020B210(p, D_00266060, 0x20042D05A1321F80LL, 8) != 0) {
            *(short *)(p + 0x1C) = 0;
            p[5] = p[5] + 1;
            func_0020BBE0(p, p[0x1A]);
        } else if (p[0x18] != 0 && (D_00810E74 & 0x40)) {
            p[6] = 1;
            kind = p[0x50 + func_0020BEF0(p)];
            if (kind == 2) {
                *(char **)(p + 0x30) = func_00185420(kind + 0x1E);
                if (*(char **)(p + 0x30) != 0) {
                    if (D_0081085C == 0.0f && !(D_00810858 < 100.0f)) {
                        func_0020CD80();
                        D_002821B4 = 0;
                        D_002821B8 = 0x1A;
                        p[5] = 6;
                        p[6] = 0xF0;
                    } else {
                        p[5] = 4;
                        p[6] = 1;
                        func_0020CD40();
                    }
                } else {
                    func_0020CD80();
                    D_002821B4 = 0;
                    D_002821B8 = 0x19;
                    p[5] = 6;
                    p[6] = 0xF0;
                }
            } else if ((unsigned int)(kind - 3) < 2) {
                if (D_0081085C != 0.0f && D_00810707 == 0) {
                    p[5] = 4;
                    p[6] = 1;
                    func_0020CD40();
                } else if (D_00810707 == 1) {
                    if (!(D_00810858 < 60.0f)) {
                        func_0020CD80();
                        D_002821B4 = 0;
                        D_002821B8 = 1;
                        p[5] = 6;
                        p[6] = 0xF0;
                    } else {
                        p[5] = 4;
                        p[6] = 1;
                        func_0020CD40();
                    }
                } else if (!(D_00810858 < 100.0f)) {
                    func_0020CD80();
                    D_002821B4 = 0;
                    D_002821B8 = 1;
                    p[5] = 6;
                    p[6] = 0xF0;
                } else {
                    p[5] = 4;
                    p[6] = 1;
                    func_0020CD40();
                }
            } else if (D_00810707 == 1) {
                if (!(D_00810858 < 60.0f)) {
                    func_0020CD80();
                    D_002821B4 = 0;
                    D_002821B8 = 0x1B;
                    p[5] = 6;
                    p[6] = 0xF0;
                } else {
                    p[5] = 4;
                    p[6] = 1;
                    func_0020CD40();
                }
            } else if (!(D_00810858 < 100.0f)) {
                func_0020CD80();
                D_002821B4 = 0;
                D_002821B8 = 0x1B;
                p[5] = 6;
                p[6] = 0xF0;
            } else {
                p[5] = 4;
                p[6] = 1;
                func_0020CD40();
            }
        }
        func_0020B0D0(p, D_00265FF0);
        break;
    case 2:
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        if (func_0020BC50(p, D_00266060, 0x20042D05A1321F80LL, 8) != 0) {
            p[5] = p[5] - 1;
        }
        func_0020B0D0(p, D_00265FF0);
        break;
    case 6:
        D_002821B4 = 1;
        D_00282240 = 5;
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        func_0020B210(p, D_00266060, 0x20042D05A1321F80LL, 0x608);
        func_0020B0D0(p, D_00265FF0);
        if (!(D_00810E74 & 0x60)) {
            n = p[6] - 1;
            p[6] = n;
            if ((n & 0xFF) != 0) {
                break;
            }
        }
        if (D_00810E74 & 0x60) {
            func_0020CD60();
        }
        D_002821B4 = 0;
        D_008106B0 = 0;
        D_00282240 = 3;
        p[5] = 1;
        break;
    case 3:
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        func_0020B210(p, D_00266060, 0x20042D05A1321F80LL, 0x408);
        func_0020B0D0(p, D_00265FF0);
        n = p[6] - 1;
        p[6] = n;
        if ((n & 0xFF) != 0) {
            if (!(D_00810E74 & 0x5060)) {
                break;
            }
        }
        if (D_00810E74 & 0x5060) {
            func_0020CD60();
        }
        D_008106B0 = 0;
        p[5] = 1;
        D_00282240 = 3;
        break;
    case 4:
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        func_0020B210(p, D_00266060, 0x20042D05A1321F80LL, 0x408);
        func_0020B0D0(p, D_00265FF0);
        D_002821B4 = 1;
        D_00282240 = 5;
        if (p[0x50 + idx] == 2) {
            D_002821B8 = 0x12;
        } else {
            D_002821B8 = 0x18;
        }
        func_001FCF10();
        func_00207D00(1, 3);
        ev = D_00810E74;
        if (ev & 0x8000) {
            if (p[6] != 0) {
                p[6] = p[6] - 1;
                func_0020CDA0(p);
            }
        } else if (ev & 0x2000) {
            if (p[6] == 0) {
                p[6] = p[6] + 1;
                func_0020CDA0(p);
            }
        }
        func_0020CCB0(p);
        ev = D_00810E74;
        if (ev & 0x40) {
            if (p[6] != 0) {
                D_002821B4 = 0;
                D_00282240 = 3;
                p[5] = 1;
                func_0020CD60();
                break;
            }
            func_0020CD40();
            kind = p[0x50 + func_0020BEF0(p)];
            D_008106B0 = 1;
            switch (kind) {
            case 0:
                *(float *)(p + 0x34) = 30.0f + D_00810858;
                if (D_00810707 == 1) {
                    if (!(*(float *)(p + 0x34) < 60.0f)) {
                        *(float *)(p + 0x34) = 60.0f;
                    }
                } else if (!(*(float *)(p + 0x34) < 100.0f)) {
                    *(float *)(p + 0x34) = 100.0f;
                }
                p[5] = 5;
                *(short *)(p + 0x3C) = 3;
                p[6] = 0;
                break;
            case 1:
                *(float *)(p + 0x34) = 100.0f;
                if (D_00810707 == 1) {
                    if (!(*(float *)(p + 0x34) < 60.0f)) {
                        *(float *)(p + 0x34) = 60.0f;
                    }
                } else if (!(*(float *)(p + 0x34) < 100.0f)) {
                    *(float *)(p + 0x34) = 100.0f;
                }
                p[5] = 5;
                *(short *)(p + 0x3C) = 3;
                p[6] = 0;
                break;
            case 2:
                D_008106B0 = 2;
                *(float *)(p + 0x34) = 100.0f;
                *(float *)(p + 0x38) = 0.0f;
                p[0x10] = 8;
                rec = *(char **)(p + 0x30);
                rec[0xA] = 1;
                rec[0xB] = 5;
                *(char *)0x70003B8D = 3;
                func_0015C750(&D_008102B0, 3, 1, rec);
                break;
            case 3:
                if (D_00810707 == 1) {
                    *(float *)(p + 0x34) = 30.0f + D_00810858;
                    if (!(*(float *)(p + 0x34) < 60.0f)) {
                        *(float *)(p + 0x34) = 60.0f;
                    }
                    *(float *)(p + 0x38) = D_0081085C;
                } else {
                    *(float *)(p + 0x34) = 30.0f + D_00810858;
                    if (!(*(float *)(p + 0x34) < 100.0f)) {
                        *(float *)(p + 0x34) = 100.0f;
                    }
                    *(float *)(p + 0x38) = D_0081085C - 30.0f;
                    if (*(float *)(p + 0x38) < 0.0f) {
                        *(float *)(p + 0x38) = 0.0f;
                    }
                }
                p[0x10] = 8;
                break;
            case 4:
                if (D_00810707 == 1) {
                    *(float *)(p + 0x34) = 60.0f;
                    *(float *)(p + 0x38) = D_0081085C;
                } else {
                    *(float *)(p + 0x34) = 100.0f;
                    *(float *)(p + 0x38) = 0.0f;
                }
                p[0x10] = 8;
                break;
            }
            func_001C47E0(kind + *(short *)(p + 0x1E), 1);
            func_00215FE0(p);
            n = p[0x18];
            if (n < 4) {
                j = n - 1;
                if (!(p[0x17] < j)) {
                    p[0x17] = j;
                }
            }
            break;
        }
        if (ev & 0x20) {
            func_0020CD60();
            D_002821B4 = 0;
            D_00282240 = 3;
            p[5] = 1;
        }
        break;
    case 5:
        func_0020A7A0(0x200434C59D422150LL);
        func_0020AE40(p, D_00265FF0, 8);
        func_0020B210(p, D_00266060, 0x20042D05A1321F80LL, 0x608);
        func_0020B0D0(p, D_00265FF0);
        *(short *)(p + 0x3C) = *(short *)(p + 0x3C) - 1;
        if ((*(int *)0x70003B64 % 10) == 0) {
            func_001FB9F0(0xE, 0x1000, 0x1000, 0x1000);
        }
        if (*(float *)(p + 0x34) != D_00810858) {
            if (*(short *)(p + 0x3C) == 0) {
                D_00810858 += 1.0f;
            }
        } else {
            p[6] = 1;
        }
        if (p[6] == 1 || (D_00810E74 & 0x870)) {
            lvl = *(float *)(p + 0x34);
            D_00810858 = lvl;
            D_008104D0 = lvl;
            func_0015C700(&D_008102B0);
            if (D_00810E74 & 0x870) {
                func_0020CD40();
            }
            D_008106B0 = 0;
            p[5] = 1;
            p[6] = 0;
            D_002821B4 = 0;
            D_00282240 = 3;
            break;
        }
        if (*(short *)(p + 0x3C) == 0) {
            *(short *)(p + 0x3C) = 3;
        }
        break;
    }
}
