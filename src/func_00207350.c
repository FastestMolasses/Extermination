// NEARMISS func_00207350  (vram 0x00207350, 0x69C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 100.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// MATCHED, but with a BUILD PREREQUISITE the parent must apply or the claim will not reproduce: the expected object must spell the scratchpad word 0x70003B64 as the extern D_70003B64 (%hi/%lo). Two one-line additions: (1) tools/decomp/build.py -- add "0x70003B64" to _SPAD_SYMS; (2) config/SCUS_971....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// SPADSYM: 0x70003B64
//
// func_00207350 — byte-identical match (objdiff 100.0, mwcc 2.3.3 / 000906).
//
// BUILD PREREQUISITE. Two one-line additions on the target-object side:
//   1. tools/decomp/build.py: add "0x70003B64" to _SPAD_SYMS.
//   2. config/SCUS_971.12.lcf: add `D_70003B64 = 0x70003B64;`.
// This is exactly the treatment 0x70003B6C and 0x70003B8D already get. Blast
// radius is one other file: src/func_001BF6B0.c names D_70003B64 in a COMMENT,
// so the mention-based opt-in would symbolize its target too — either scope the
// opt-in to a "// SPADSYM:" directive (preferred, see func_0014AFA0) or reword
// that comment. func_001BF6B0 is a NEARMISS, not part of matched_code.
//
// WHY IT IS NEEDED. 0x70003B64 occurs exactly once in this function, at the top
// of the block the `p[5]==4` timer branch jumps over. With the literal spelling,
// mwcc peels that block's bare `lui at,0x7000` into the branch's empty delay slot
// and retargets +4; the original has a nop. As an extern the lui carries an
// R_MIPS_HI16 relocation, which mwcc will not speculate, and the slot stays nop.
// Declared as an INCOMPLETE ARRAY on purpose: `extern int D_70003B64;` is a
// small-data candidate under -sdatathreshold 8 and comes out as %gp_rel.
//
// Sweep (objdiff, expected object symbolized as above):
//   mwcc 991202 = 91.30   mwcc233 000906 = 100.0   mwcc24 001213 = 97.35
//   mwcc30 / mwcc301 are NOT installed on this machine — not swept.

// SEMANTICS: On-screen numeric keypad / code-entry panel state machine.
// p[4] is the outer state, p[5] the entry sub-state dispatched through
// jtbl_00273530 (9 entries, 0..8), p[6] the intro sub-state, p[0xA] the number
// of characters typed, p[0xB] the panel/slot id (derived from info[0x10]-4 where
// info = *(void **)(p+0x20)), *(short *)(p+0x28) the 0..10 grid cursor,
// *(short *)(p+0x2A) a frame timer and p+0x60 a 16-byte text buffer.
// D_00810E74 is the pad-button bitmask; D_00275860[slot] / D_00275858[slot] are
// the per-slot widget and expected-string handles.
//   outer 0    - reset: cursor = 9, timer = 0, no characters typed, slot id from
//                info[0x10]-4, zero the 16-byte buffer, advance and clear p[5], p[6].
//   outer 1    - func_002079F0(p, widget) then dispatch on p[5]:
//      p[5]==0 - intro sub-machine on p[6]: 0 -> D_002821B0=3, D_002821B4=1 and
//                clear D_00282228/D_002821B8/D_002821C0, advance; 1 -> advance when
//                D_002821C0 signals ready; 2 -> the live keypad. Pad 0x1000/0x4000
//                move the cursor by -3/+3 (clamped to 10) and pad 0x2000/0x8000 by
//                +1/-1 while skipping the row edges (2/5/8/10 and 0/3/6/9), each
//                move calling func_002072A0. Pad 0x810 cancels: func_0020CD60 and
//                advance p[4]. Pad 0x40 confirms: on cursor 10 (the ENTER cell)
//                advance p[5], clear p[6], NUL-terminate the buffer and play sound
//                0x8C8; otherwise, with fewer than 8 characters typed, append
//                D_00265010[cursor] + '0' and play 0x8C6. Pad 0x20 backspaces and
//                plays 0x8C7. Ends with func_00207BB0(p, widget).
//      p[5]==1 - compare the typed buffer against D_00275858[slot]: a match takes
//                p[5] to 3 (slot 0, also setting bit 0x20 in D_00810845) or 6,
//                arms a 0x3C timer and sets D_002821B8=4; a mismatch sets
//                D_002821B8=2, plays 0x8CB and advances p[5]. Both then redraw.
//      p[5]==2 - when D_002821C0 signals, arm 0x50 ticks and go to p[5]=7.
//      p[5]==3 - when D_002821C0 signals, go to p[5]=8 with a 0x5A timer.
//      p[5]==4 - on timer expiry play 0x8CA, advance p[5] and re-arm 0xF0; each
//                frame func_00207CD0 (unless scratchpad flag 0x20 at 0x70003B64)
//                then func_00207CA0.
//      p[5]==5 - on timer expiry advance p[4]; always func_00207CD0 + func_00207CA0.
//      p[5]==6 - advance p[4] when D_002821C0 signals.
//      p[5]==7 - on timer expiry reset p[4] to 0.
//      p[5]==8 - on timer expiry go back to p[5]=4 with D_002821B8=6 and a 0x12C timer.
//   outer 2/3  - shared shutdown: D_002821B4 = 2, func_001AEDB0(0, state, info),
//                D_008106C5 = 0xFF, func_001AFF90(p).
//
// NOTE: the D_0028xxxx globals are declared `volatile` and the non-%gp_rel
// globals as incomplete arrays on purpose.

extern int func_00123020();
extern int func_001AEDB0();
extern int func_001AFF90();
extern int func_001FB9F0();
extern int func_002072A0();
extern int func_002079F0();
extern int func_00207BB0();
extern int func_00207CA0();
extern int func_00207CD0();
extern int func_0020CD60();

extern unsigned char D_00265010[];
extern int D_00275858[2];
extern int D_00275860[2];
extern volatile int D_002821B0[];
extern volatile int D_002821B4[];
extern volatile int D_002821B8[];
extern volatile int D_002821C0[];
extern volatile int D_00282228[];
extern unsigned char D_008106C5[];
extern unsigned char D_00810845[];
extern unsigned short D_00810E74[];
extern int D_70003B64[];

void func_00207350(unsigned char *p) {
    unsigned char state = p[4];
    unsigned char *info = *(unsigned char **)(p + 0x20);
    unsigned char *buf = p + 0x60;
    int i;
    short n;
    short c;

    switch (state) {
    case 0:
        *(short *)(p + 0x28) = 9;
        *(short *)(p + 0x2A) = 0;
        p[0xA] = 0;
        p[0xB] = info[0x10] - 4;
        for (i = 0; i < 0x10; i++) {
            buf[i] = 0;
        }
        p[4] = p[4] + 1;
        p[5] = 0;
        p[6] = 0;
        break;
    case 1:
        func_002079F0(p, D_00275860[p[0xB]]);
        switch (p[5]) {
        case 0:
            switch (p[6]) {
            case 0:
                D_002821B0[0] = 3;
                D_002821B4[0] = 1;
                D_00282228[0] = 0;
                D_002821B8[0] = 0;
                D_002821C0[0] = 0;
                p[6] = p[6] + 1;
                break;
            case 1:
                if (D_002821C0[0] != 0) {
                    p[6] = p[6] + 1;
                }
                break;
            case 2:
                if (D_00810E74[0] & 0x1000) {
                    n = *(short *)(p + 0x28);
                    if (n >= 3) {
                        *(short *)(p + 0x28) = n - 3;
                        func_002072A0();
                    }
                } else if (D_00810E74[0] & 0x4000) {
                    n = *(short *)(p + 0x28);
                    if (n < 9) {
                        *(short *)(p + 0x28) = n + 3;
                        if (*(short *)(p + 0x28) > 0xA) {
                            *(short *)(p + 0x28) = 0xA;
                        }
                        func_002072A0();
                    }
                }
                if (D_00810E74[0] & 0x2000) {
                    n = *(short *)(p + 0x28);
                    if (n != 2 && n != 5 && n != 8 && n != 0xA) {
                        func_002072A0();
                        *(short *)(p + 0x28) = *(short *)(p + 0x28) + 1;
                    }
                } else if (D_00810E74[0] & 0x8000) {
                    n = *(short *)(p + 0x28);
                    if (n != 0 && n != 3 && n != 6 && n != 9) {
                        func_002072A0();
                        *(short *)(p + 0x28) = *(short *)(p + 0x28) - 1;
                    }
                }
                if (D_00810E74[0] & 0x810) {
                    func_0020CD60();
                    p[4] = p[4] + 1;
                } else if (D_00810E74[0] & 0x40) {
                    n = *(short *)(p + 0x28);
                    if (n == 0xA) {
                        p[5] = p[5] + 1;
                        p[6] = 0;
                        buf[p[0xA]] = 0;
                        func_001FB9F0(0x8C8, 0x1000, 0x1000, 0x1000);
                    } else if (p[0xA] < 8) {
                        c = D_00265010[n];
                        buf[p[0xA] & 0xFF] = c + 0x30;
                        p[0xA] = p[0xA] + 1;
                        func_001FB9F0(0x8C6, 0x1000, 0x1000, 0x1000);
                    }
                } else if (D_00810E74[0] & 0x20) {
                    if (p[0xA] != 0) {
                        p[0xA] = p[0xA] - 1;
                        func_001FB9F0(0x8C7, 0x1000, 0x1000, 0x1000);
                    }
                }
                func_00207BB0(p, D_00275860[p[0xB]]);
                break;
            }
            break;
        case 1:
            if (func_00123020(buf, D_00275858[p[0xB]]) == 0) {
                if (p[0xB] == 0) {
                    p[5] = 3;
                    D_00810845[0] = D_00810845[0] | 0x20;
                } else {
                    p[5] = 6;
                }
                *(short *)(p + 0x2A) = 0x3C;
                D_002821B8[0] = 4;
                D_002821C0[0] = 0;
            } else {
                D_002821B8[0] = 2;
                D_002821C0[0] = 0;
                func_001FB9F0(0x8CB, 0x1000, 0x1000, 0x1000);
                p[5] = p[5] + 1;
            }
            func_00207BB0(p, D_00275860[p[0xB]]);
            break;
        case 2:
            if (D_002821C0[0] != 0) {
                *(short *)(p + 0x2A) = 0x50;
                p[5] = 7;
            }
            break;
        case 3:
            if (D_002821C0[0] != 0) {
                p[5] = 8;
                *(short *)(p + 0x2A) = 0x5A;
            }
            break;
        case 4:
            if (--*(short *)(p + 0x2A) == 0) {
                func_001FB9F0(0x8CA, 0x1000, 0x1000, 0x1000);
                p[5] = p[5] + 1;
                *(short *)(p + 0x2A) = 0xF0;
            }
            if (!(D_70003B64[0] & 0x20)) {
                func_00207CD0(D_00275860[p[0xB]]);
            }
            func_00207CA0(D_00275860[p[0xB]]);
            break;
        case 5:
            if (--*(short *)(p + 0x2A) == 0) {
                p[4] = p[4] + 1;
            }
            func_00207CD0(D_00275860[p[0xB]]);
            func_00207CA0(D_00275860[p[0xB]]);
            break;
        case 6:
            if (D_002821C0[0] != 0) {
                p[4] = p[4] + 1;
            }
            break;
        case 7:
            if (--*(short *)(p + 0x2A) == 0) {
                p[4] = 0;
            }
            break;
        case 8:
            if (--*(short *)(p + 0x2A) == 0) {
                p[5] = 4;
                D_002821B8[0] = 6;
                D_002821C0[0] = 0;
                *(short *)(p + 0x2A) = 0x12C;
            }
            break;
        }
        break;
    case 2:
    case 3:
        D_002821B4[0] = 2;
        func_001AEDB0(0, state, info);
        D_008106C5[0] = 0xFF;
        func_001AFF90(p);
        break;
    }
}
