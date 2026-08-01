// NEARMISS func_002149F0  (vram 0x002149F0, 0xE80 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 98.694% (mwcc233 -O4,p -sdatathreshold 0); ~24 of 928 instructions differ, in five clusters, all body-correct. (1) REGALLOC PERMUTATION on `p[0x12] - (rec[0x34] << 1)` at two sites (8 instrs): identical sequence, but the target puts the `lh rec[0x34]` result in v1/v0 and `lbu p[0x12]` in a0/v1, m...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: memory-card / save-data browser page of the system menu. `p` is the
// menu-page object; p[5] is the page sub-state, dispatched through a 9-entry jump
// table (jtbl_00273720). The case bodies are emitted 0,1,2,8,3,4,5,6,7, so the
// source case order must match (the table is positional). The shared per-frame
// work is the func_0020A7A0 / func_0020AE40 / func_0020B210 / func_0020B0D0
// quartet (GIF tag, window open, list draw with a per-state flag word, close).
//   0  build the entry list from the three availability flags D_00810C7F/80/81
//      (kinds 0/1/2 appended at p[0x50+n], count in p[0x18]); reset the HUD state
//      words D_002821B0/B4/40; then branch on the pending request D_008106B0:
//        == 6            -> take D_008106D0 as the active record and pick the
//                           "not enough blocks" (state 5) or normal (state 4) path
//                           from D_00810CB2 vs 2 * record[0x34];
//        D_008106B1&0x80 -> direct load; icon id p[0x13] from the block count
//                           (4->0xA, 6->0xC, 16->0xE, 24->0x10, else 8);
//        D_008106B1&0x40 -> direct save, state 5 or 4 with icon 6;
//        otherwise       -> select the entry whose kind matches D_008106B1-0x1B,
//                           set the block budget (0xC / 0x24 / 0x30), state 3.
//      With no pending request, advance to state 1 and fall through.
//   1  idle list page: on cancel (0x20) close the page (p[1]=3); otherwise draw,
//      and on confirm (0x40) resolve the highlighted entry through func_00185420
//      into a record pointer, then pick state 4/5 and the icon id. A null record
//      means "no card": state 8.
//   2  page-close animation (func_0020BC50); steps back to state 1 when finished.
//   8  error banner (message 0x19) for 240 frames or until a button.
//   3  short banner (240 frames) before returning to the list.
//   4  confirm dialog: cursor through func_0020CDA0, confirm/cancel through
//      D_00810E74, then state 7 (icon 6 = delete) or state 6 / state 5.
//   5  card-access banner; releases back to the list on 0x870 or on the timer.
//   6  block-count roll-DOWN animation: steps D_00810CB2 down by 2 every 30 frames
//      until it reaches p[0x12] - 2 * record[0x34], then finishes the operation
//      (marks the record 1/5 and pokes the 0x70003B8D scratchpad flag).
//   7  block-count roll-UP animation: the mirror of state 6, up to D_00810CB7.
// D_00810CB2 is written as a 16-bit block counter but also read as its low byte,
// hence the union (that is what reproduces the target's per-access %hi/%lo pair).
// D_00810E74 = this frame's button/event bits.

extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_00282240;
extern unsigned char D_008106B0;
extern unsigned char D_008106B1;
extern unsigned char D_008106C5;
extern char *D_008106D0;
extern unsigned char D_00810C7F;
extern unsigned char D_00810C80;
extern unsigned char D_00810C81;
extern union { unsigned char b; short h; } D_00810CB2;
extern unsigned char D_00810CB7;
extern unsigned short D_00810E74;
extern char D_00265C50[];
extern char D_00265CD0[];

extern char *func_00185420(int id);
extern void func_001FB9F0(int a, int b, int c, int d);
extern void func_001FCF10();
extern void func_00207D00(int a, int b);
extern void func_0020A7A0(long long tag);
extern void func_0020AE40(void *p, void *q, int n);
extern void func_0020B0D0(void *p, void *q);
extern int func_0020B210(void *p, void *q, long long tag, int flags);
extern void func_0020BBE0(void *p, int n);
extern int func_0020BC50(void *p, void *q, long long tag, int flags);
extern void func_0020CCB0(void *p);
extern void func_0020CD40();
extern void func_0020CD60();
extern void func_0020CD80();
extern void func_0020CDA0(void *p);

void func_002149F0(unsigned char *p) {
    char *rec;
    int slot;
    int i;
    int n;
    int sel;
    int flg;
    int ev;

    switch (p[5]) {
    case 0:
        p[0x17] = 0;
        p[0x19] = 0;
        p[0x18] = 0;
        p[0x1A] = 0;
        D_002821B0 = 4;
        D_002821B4 = 0;
        D_00282240 = 3;
        *(short *)(p + 0x1E) = 0x1B;
        if (D_00810C81 != 0) {
            p[0x50 + p[0x18]++] = 2;
        } else if (D_00810C80 != 0) {
            p[0x50 + p[0x18]++] = 1;
        } else if (D_00810C7F != 0) {
            p[0x50 + p[0x18]++] = 0;
        }
        if (D_008106B0 != 0) {
            if ((D_008106B0 & 0xFF) == 6) {
                p[0x12] = D_00810CB2.b;
                p[0x13] = 8;
                rec = D_008106D0;
                *(char **)(p + 0x30) = rec;
                if (D_00810CB2.h < (*(short *)(rec + 0x34) << 1)) {
                    p[5] = 5;
                    p[6] = 0xF0;
                } else {
                    p[5] = 4;
                    p[6] = 1;
                }
                break;
            }
            flg = D_008106B1;
            if (flg & 0x80) {
                rec = D_008106D0;
                *(char **)(p + 0x30) = rec;
                D_008106B0 = 0;
                p[5] = 4;
                p[6] = 1;
                p[0x12] = D_00810CB2.b;
                switch (*(short *)(rec + 0x34)) {
                default:
                    p[0x13] = 8;
                    break;
                case 4:
                    p[0x13] = 0xA;
                    break;
                case 6:
                    p[0x13] = 0xC;
                    break;
                case 16:
                    p[0x13] = 0xE;
                    break;
                case 24:
                    p[0x13] = 0x10;
                    break;
                }
                break;
            }
            if (flg & 0x40) {
                *(char **)(p + 0x30) = D_008106D0;
                D_008106B0 = 0;
                if (D_00810CB2.h == D_00810CB7) {
                    p[5] = 5;
                    p[6] = 0x78;
                    p[0x13] = 6;
                } else {
                    p[5] = 4;
                    p[6] = 1;
                    p[0x13] = 6;
                }
                break;
            }
            p[0x1B] = flg - 0x1B;
            n = p[0x18];
            i = 0;
            goto scan_test;
        scan_body:
            if (p[0x50 + i] == p[0x1B]) {
                p[0x17] = i;
                D_00282240 = 4;
                goto scan_done;
            }
            i++;
        scan_test:
            if (i < n) {
                goto scan_body;
            }
        scan_done:
            if (p[0x1B] == 0) {
                D_00810CB2.h = 0xC;
                D_00810CB7 = 0xC;
            } else if (p[0x1B] == 1) {
                D_00810CB2.h = 0x24;
                D_00810CB7 = 0x24;
            } else {
                D_00810CB2.h = 0x30;
                D_00810CB7 = 0x30;
            }
            D_008106B0 = 0;
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
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        if (func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 2) != 0) {
            *(short *)(p + 0x1C) = 0;
            p[5] = p[5] + 1;
            func_0020BBE0(p, p[0x1A]);
        } else if (p[0x18] != 0 && (D_00810E74 & 0x40)) {
            *(char **)(p + 0x30) = func_00185420(p[0x50 + p[0x17]] + 0x1B);
            if (*(char **)(p + 0x30) != 0) {
                func_0020CD40();
                if (*(unsigned char *)(*(char **)(p + 0x30) + 3) == 0x2C) {
                    if (D_00810CB2.h == D_00810CB7) {
                        p[5] = 5;
                        p[6] = 0x78;
                        p[0x13] = 6;
                    } else {
                        p[5] = 4;
                        p[6] = 1;
                        p[0x13] = 6;
                    }
                } else {
                    p[5] = 4;
                    p[6] = 1;
                    rec = *(char **)(p + 0x30);
                    p[0x12] = D_00810CB2.b;
                    switch (*(short *)(rec + 0x34)) {
                    default:
                        p[0x13] = 8;
                        break;
                    case 4:
                        p[0x13] = 0xA;
                        break;
                    case 6:
                        p[0x13] = 0xC;
                        break;
                    case 16:
                        p[0x13] = 0xE;
                        break;
                    case 24:
                        p[0x13] = 0x10;
                        break;
                    }
                }
            } else {
                func_0020CD80();
                D_002821B4 = 0;
                p[5] = 8;
                p[6] = 0xF0;
            }
        }
        func_0020B0D0(p, D_00265C50);
        break;
    case 2:
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        if (func_0020BC50(p, D_00265CD0, 0x20042D05A1322000LL, 2) != 0) {
            p[5] = p[5] - 1;
        }
        func_0020B0D0(p, D_00265C50);
        break;
    case 8:
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = 0x19;
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x602);
        func_0020B0D0(p, D_00265C50);
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
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x402);
        func_0020B0D0(p, D_00265C50);
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
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x402);
        func_0020B0D0(p, D_00265C50);
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = p[0x13];
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
                if (D_008106B0 == 6) {
                    D_008106C5 = 0xFF;
                }
                break;
            }
            if (p[0x13] == 6) {
                p[0x12] = D_00810CB2.b;
                p[5] = 7;
                p[6] = 0;
                *(short *)(p + 0x3C) = 3;
            } else {
                if (D_00810CB2.h < (*(short *)(*(char **)(p + 0x30) + 0x34) << 1)) {
                    p[5] = 5;
                    p[6] = 0xF0;
                    func_0020CD60();
                    break;
                }
                p[5] = 6;
                *(short *)(p + 0x3C) = 1;
            }
            if (D_008106B0 != 6) {
                D_008106B0 = 1;
            } else {
                D_002821B4 = 0;
                p[1] = 6;
                p[2] = 0;
                p[3] = 0;
                p[4] = 0;
                p[5] = 0;
            }
            func_0020CD40();
            break;
        }
        if (D_008106B0 == 6) {
            if (ev & 0x830) {
                D_002821B4 = 0;
                func_0020CD60();
                D_008106C5 = 0xFF;
            }
            break;
        }
        if (ev & 0x20) {
            D_002821B4 = 0;
            D_00282240 = 3;
            p[5] = 1;
            func_0020CD60();
        }
        break;
    case 5:
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x402);
        func_0020B0D0(p, D_00265C50);
        D_002821B4 = 1;
        D_00282240 = 5;
        D_002821B8 = p[0x13] + 1;
        if (D_008106B0 == 6) {
            if (D_00810E74 & 0x870) {
                D_002821B4 = 0;
                D_008106C5 = 0xFF;
                func_0020CD60();
                break;
            }
            n = p[6] - 1;
            p[6] = n;
            if ((n & 0xFF) == 0) {
                D_002821B4 = 0;
                D_008106C5 = 0xFF;
            }
        } else {
            if (D_00810E74 & 0x60) {
                D_002821B4 = 0;
                D_00282240 = 3;
                p[5] = 1;
                func_0020CD60();
                break;
            }
            n = p[6] - 1;
            p[6] = n;
            if ((n & 0xFF) == 0) {
                D_002821B4 = 0;
                D_00282240 = 3;
                p[5] = 1;
            }
        }
        break;
    case 6:
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x402);
        func_0020B0D0(p, D_00265C50);
        D_002821B4 = 0;
        rec = *(char **)(p + 0x30);
        {
            short t = *(short *)(p + 0x3C) - 1;
            *(short *)(p + 0x3C) = t;
            if (t == 0) {
                *(short *)(p + 0x3C) = 0x1E;
                if (D_00810CB2.h == (p[0x12] - (*(short *)(rec + 0x34) << 1))) {
                    D_008106C5 = 0xFF;
                    rec[0xA] = 1;
                    rec[0xB] = 5;
                    *(char *)0x70003B8D = 3;
                    break;
                }
                D_00810CB2.h = D_00810CB2.h - 2;
                func_001FB9F0(6, 0x1000, 0x1000, 0x1000);
            }
        }
        if (D_00810E74 & 0x870) {
            D_00810CB2.h = p[0x12] - (*(short *)(rec + 0x34) << 1);
            func_0020CD40();
            D_008106C5 = 0xFF;
            rec[0xA] = 1;
            rec[0xB] = 5;
            *(char *)0x70003B8D = 3;
        }
        break;
    case 7:
        func_0020A7A0(0x20043C859D422150LL);
        func_0020AE40(p, D_00265C50, 2);
        func_0020B210(p, D_00265CD0, 0x20042D05A1322000LL, 0x402);
        func_0020B0D0(p, D_00265C50);
        *(short *)(p + 0x3C) = *(short *)(p + 0x3C) - 1;
        if (D_00810CB7 != D_00810CB2.h) {
            if (*(short *)(p + 0x3C) == 0) {
                D_00810CB2.h = D_00810CB2.h + 2;
                func_001FB9F0(6, 0x1000, 0x1000, 0x1000);
            }
        } else {
            p[6] = 1;
        }
        D_002821B4 = 0;
        sel = p[6];
        if (sel == 1 || (D_00810E74 & 0x870)) {
            rec = *(char **)(p + 0x30);
            D_00810CB2.h = D_00810CB7;
            func_0020CD40();
            D_008106C5 = 0xFF;
            rec[0xA] = 1;
            rec[0xB] = 5;
            *(char *)0x70003B8D = 3;
            break;
        }
        if (*(short *)(p + 0x3C) == 0) {
            *(short *)(p + 0x3C) = 0x14;
        }
        break;
    }
}
