// NEARMISS func_002149F0  (vram 0x002149F0, 0xE80 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate (round 6, 2026-09-23):
// One four-instruction cluster: in case 6 the compare D_00810CB2.h == p[0x12] - (rec[0x34] << 1)
// loads rec[0x34] into v1 and p[0x12] into a0 in the target; mwcc swaps them. Round 6:
// func_0020CDA0 is (void) with no argument, D_002821B0/B4 volatile, (p + 0x50)[i] in the scan,
// case 2: shares the default arm of both p[0x34] switches (restores the target's dead li and scan-
// loop nop), and the store site stages rec[0x34] in n.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: the status screen's ITEM > BATTERY sub-module page (ITEM
// sub-module 0x21, FINDINGS "STATUS SUB-PAGES"; the port's
// tools/test_battery_reference.py executes this function). NOT a memory-card /
// save-data browser (the old label). `p` is the page object; p[5] is the page
// sub-state, dispatched through a 9-entry jump table (jtbl_00273720). The case
// bodies are emitted 0,1,2,8,3,4,5,6,7, so the source case order must match
// (the table is positional). The shared per-frame work is the func_0020A7A0 /
// func_0020AE40 / func_0020B210 / func_0020B0D0 quartet (GIF tag, window open,
// list draw with a per-state flag word, close). D_00810CB2 is the battery
// charge (half-units), D_00810CB7 the capacity, D_00810C7F/80/81 the owned
// counts of battery items 0x1B/0x1C/0x1D, p[0x13] the message-bank group-5
// line shown by states 4/5 (D_002821B8), record +0x34 the owner's cost.
//   0  list the highest owned battery kind (2/1/0) at p[0x50]; reset the
//      message state D_002821B0/B4/40; then branch on the pending request
//      D_008106B0:
//        == 6            -> take the owner record D_008106D0 and pick the
//                           insufficient-charge result (state 5) or the
//                           confirmation (state 4) from D_00810CB2 vs
//                           2 * record[0x34];
//        D_008106B1&0x80 -> confirmation for owner D_008106D0; line p[0x13]
//                           from the cost (4->0xA, 6->0xC, 16->0xE, 24->0x10,
//                           else 8);
//        D_008106B1&0x40 -> recharge path for owner D_008106D0 (line 6; state
//                           5 when charge == capacity, else 4);
//        otherwise       -> acquisition of battery item D_008106B1 (kind =
//                           type-0x1B): select it, set charge and capacity to
//                           0xC / 0x24 / 0x30, notice state 3.
//      With no pending request, advance to state 1 and fall through.
//   1  battery list: on cancel (0x20) back to the ITEM page (p[1]=3);
//      otherwise draw, and on confirm (0x40) look up the use target for item
//      0x1B+kind through func_00185420; record type 0x2C takes the recharge
//      path, others the cost confirmation; no target -> func_0020CD80 and
//      state 8.
//   2  list-close animation (func_0020BC50); steps back to state 1.
//   8  "no target" line 0x19 for 240 frames or until 0x60.
//   3  acquisition notice (240 frames or input 0x5060) before the list.
//   4  Yes/No confirmation (default No; 0x8000/0x2000 move the cursor via
//      func_0020CDA0); Yes -> state 7 (recharge, line 6) or state 6 (discharge;
//      insufficient charge -> state 5); under request 6 a sufficient-charge
//      Yes instead leaves the page with p[1] = 6 (status phase 6). No/cancel
//      returns to the list, or for request 6 sets D_008106C5 = 0xFF.
//   5  result line p[0x13]+1; releases on 0x870/0x60 or on the timer.
//   6  discharge: every 30 frames subtract 2 half-units (sound 6) until the
//      charge reaches p[0x12] - 2 * record[0x34] (0x870 finishes at once), then
//      mark the owner record +0xA=1/+0xB=5, set D_008106C5 = 0xFF and poke
//      scratchpad 0x70003B8D = 3.
//   7  recharge: the mirror of state 6, up to the capacity D_00810CB7.
// D_00810CB2 is written as a 16-bit counter but also read as its low byte,
// hence the union (that is what reproduces the target's per-access %hi/%lo pair).
// D_00810E74 = this frame's button/event bits.

extern volatile int D_002821B0;
extern volatile int D_002821B4;
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
extern void func_0020CDA0(void);

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
                case 2:
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
            if ((p + 0x50)[i] == p[0x1B]) {
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
                    case 2:
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
                func_0020CDA0();
            }
        } else if (ev & 0x2000) {
            if (p[6] == 0) {
                p[6] = p[6] + 1;
                func_0020CDA0();
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
            n = *(short *)(rec + 0x34);
            D_00810CB2.h = p[0x12] - (n << 1);
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
