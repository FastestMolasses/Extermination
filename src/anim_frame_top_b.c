// NEARMISS anim_frame_top_b  (vram 0xframe_top_b, 0x594 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NOT MATCHED — jr-table dispatch (the PROVEN-unmatchable class). Top-level dispatch is `lui %hi(jtbl_0026DD30); addiu %lo; sll idx,2; addu; lw; jr` on an EXTERNAL consolidated rodata table; mwcc emits its own LOCAL `@65` table, so objdiff counts the reloc as a mismatch even though the dispatch sch...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: top-level per-frame driver for the title/attract flow. `g` is the
// global frontend record at *(void **)0x70003B6C; g[0xB] is the top-level screen
// state (0..6, dispatched through the 7-entry jump table jtbl_0026DD30) and g[0xC]
// is the per-screen sub-step. g[8]/g[9]/g[0xA]/g[0xD]/g[0x11] are secondary
// selection fields cleared on transitions.
//   state 0 : one-shot boot of the frontend -- advance g[0xB], then run the whole
//             init chain (renderer, audio bank 4, HUD, controller enable).
//   state 4 : re-entry into the menu (reload layout D_008101E0, reset g[0xB] to 1,
//             re-enable input) and then FALL THROUGH into state 1.
//   state 1 : poll the menu with func_001AE7E0():
//               1 -> "start game" : mode flag D_008106C4 = 2, fade func_001FB9F0,
//                    advance g[0xB], clear g[0x11]/g[0xC]/g[0xD].
//               2 -> "load game"  : D_008106C4 = 1, g[0xB] = 3, silence both
//                    music channels via func_00119828(ch, 0x3FFF, 0x3FFF).
//               3 -> "options"    : g[0xB] = 6, stop audio, func_001AEDB0(0).
//             otherwise (and only while the global busy flag D_00275BD8 is clear)
//             run the idle/attract tick -- func_001AE5E0 or func_001AE6B0 depending
//             on the scratchpad flag at 0x70003B8D -- then, if either demo flag
//             D_008106B9 / D_008106B8 is set and the language/region word
//             D_0028A9A0 == 2, kick the corresponding demo (func_001AD140 /
//             func_001AD010).
//   state 2 : poll the transition with func_0022A650(): 1 = back out (D_008106C4 = 0,
//             g[0xB]--, fade func_001FB9F0(0xD,...)); 2 = commit to gameplay (set up
//             g[8]=3, g[9]=5, clear g[0xA]/g[0xB]/g[0xC]/g[0xD], D_00275BE0 = 1);
//             3 = start the demo.
//   state 3 : two-step load. sub-step 0 waits for the disc-busy flag D_00282157 to
//             clear, then advances; sub-step 1 kicks the load and, if func_0020CDC0()
//             reports data present, moves to g[0xB] = 5 with timer D_008106EF = 0x46.
//   state 5 : tear the menu down and go back to state 1.
//   state 6 : options screen. sub-step 0 waits on D_00282157 then applies the option
//             D_008106CF through func_001FF030 or func_001FEFE0 depending on the
//             option class D_008106CE; sub-step 1 leaves the screen (back to state 1).
//
// Idiom notes for anyone re-attacking this: the inner sub-state tests MUST be written
// as switches with ASCENDING case labels (mwcc lowers a switch to a DESCENDING beq
// chain, which is the target's order); an if/else-if chain there emits the wrong
// polarity. The nine small absolute globals are over-declared as [8] arrays (idiom-20)
// so -sdatathreshold 4 keeps only D_00275BD8 / D_00275BE0 %gp_rel, matching the target.
extern void func_00119828(int, int, int);
extern void func_0018AB00(void);
extern void func_0018C0D0(void *, int);
extern void func_0018D7B0(void *, int);
extern void func_00199C50(void);
extern void func_001AD010(int);
extern void func_001AD140(int);
extern void func_001AE5E0(void);
extern void func_001AE6B0(void);
extern int func_001AE7E0(void);
extern void func_001AEBA0(int);
extern void func_001AEDB0(int);
extern void func_001AEE10(int, int);
extern void func_001AEE40(int);
extern void func_001AF150(void);
extern void func_001AF1C0(void);
extern void func_001AFCA0(void);
extern void func_001AFCF0(void);
extern void func_001B07C0(int);
extern void func_001B6990(void);
extern void func_001C1DC0(void);
extern void func_001C5C50(void);
extern void func_001D19E0(void);
extern void func_001D1C50(void);
extern void func_001D1EA0(int);
extern void func_001D1EF0(void);
extern void func_001D2610(float);
extern void func_001D2830(int, int);
extern void func_001E0CC0(int);
extern void func_001FABB0(void);
extern void func_001FAE70(int);
extern void func_001FB9F0(int, int, int, int);
extern void func_001FBC50(void);
extern void func_001FEFE0(int);
extern void func_001FF030(int);
extern int func_0020CDC0(void);
extern void func_0020E060(void);
extern int func_0022A650(void);

extern unsigned char D_00275BD8;
extern unsigned char D_00275BE0;
extern char D_00282157[8];
extern short D_0028A9A0[8];
extern unsigned char D_008101E0[];
extern unsigned char D_008101E4[8];
extern unsigned char D_008106B8[8];
extern unsigned char D_008106B9[8];
extern unsigned char D_008106C4[8];
extern unsigned char D_008106CE[8];
extern unsigned char D_008106CF[8];
extern unsigned char D_008106EF[8];

void anim_frame_top_b(void) {
    unsigned char *g;
    unsigned char *st;
    int r;

    g = *(unsigned char **)0x70003B6C;
    st = g + 0xB;
    switch (g[0xB]) {
    case 0:
        *st = *st + 1;
        func_001AFCA0();
        func_001AFCF0();
        func_001B07C0(0);
        func_001B6990();
        func_001D19E0();
        func_001C1DC0();
        func_00199C50();
        func_001AEE40(4);
        func_001FAE70(1);
        func_001C5C50();
        func_001D1EF0();
        return;
    case 4:
        func_001AFCF0();
        func_0018AB00();
        func_001B07C0(1);
        func_001C1DC0();
        D_008101E4[0] = 0;
        (*(unsigned char **)0x70003B6C)[0xB] = 1;
        func_0018D7B0(D_008101E0, 1);
        func_0018C0D0(D_008101E0, 1);
        func_001AEE10(4, 0);
        func_001FAE70(0);
        func_001C5C50();
        /* fallthrough */
    case 1:
        r = func_001AE7E0();
        if (r == 1) {
            D_008106C4[0] = 2;
            func_001FBC50();
            func_001FB9F0(0xC, 0x1000, 0x1000, 0x1000);
            (*(unsigned char **)0x70003B6C)[0xB] =
                (*(unsigned char **)0x70003B6C)[0xB] + 1;
            (*(unsigned char **)0x70003B6C)[0x11] = 0;
            (*(unsigned char **)0x70003B6C)[0xC] = 0;
            (*(unsigned char **)0x70003B6C)[0xD] = 0;
        } else if (r == 2) {
            func_0020E060();
            D_008106C4[0] = 1;
            (*(unsigned char **)0x70003B6C)[0xB] = 3;
            (*(unsigned char **)0x70003B6C)[0x11] = 0;
            (*(unsigned char **)0x70003B6C)[0xC] = 0;
            func_001FBC50();
            func_001FABB0();
            func_00119828(0, 0x3FFF, 0x3FFF);
            func_00119828(1, 0x3FFF, 0x3FFF);
        } else if (r == 3) {
            (*(unsigned char **)0x70003B6C)[0xB] = 6;
            (*(unsigned char **)0x70003B6C)[0xC] = 0;
            func_001FBC50();
            func_001FABB0();
            func_001AEDB0(0);
        } else if (D_00275BD8 == 0) {
            if (*(unsigned char *)0x70003B8D == 0) {
                func_001AE5E0();
            } else {
                func_001AE6B0();
            }
            if (D_008106B9[0] != 0) {
                if (D_0028A9A0[0] == 2) {
                    func_001AD140(D_0028A9A0[0]);
                }
            } else if (D_008106B8[0] != 0) {
                if (D_0028A9A0[0] == 2) {
                    func_001AD010(D_0028A9A0[0]);
                }
            }
        }
        return;
    case 2:
        r = func_0022A650();
        if (r == 1) {
            func_001AF1C0();
            D_008106C4[0] = 0;
            g = *(unsigned char **)0x70003B6C;
            g[0xB] = g[0xB] - 1;
            func_001FB9F0(0xD, 0x1000, 0x1000, 0x1000);
            func_001FAE70(0);
        } else if (r == 2) {
            func_001AEBA0(0xFF);
            func_001AF150();
            func_001D2610(0.0f);
            D_00275BE0 = 1;
            (*(unsigned char **)0x70003B6C)[8] = 3;
            (*(unsigned char **)0x70003B6C)[9] = 5;
            (*(unsigned char **)0x70003B6C)[0xA] = 0;
            (*(unsigned char **)0x70003B6C)[0xB] = 0;
            (*(unsigned char **)0x70003B6C)[0xC] = 0;
            (*(unsigned char **)0x70003B6C)[0xD] = 0;
        } else if (r == 3) {
            func_001AD140(0);
        }
        return;
    case 3:
        st = g + 0xC;
        switch (g[0xC]) {
        case 0:
            if (D_00282157[0] == 0) {
                *st = *st + 1;
            }
            break;
        case 1:
            func_001D1C50();
            func_001D2830(3, 1);
            if (func_0020CDC0() != 0) {
                func_001E0CC0(0);
                (*(unsigned char **)0x70003B6C)[0xB] = 5;
                D_008106EF[0] = 0x46;
                func_001AEDB0(0);
            }
            func_001D1EA0(0);
            break;
        }
        return;
    case 5:
        func_001AEDB0(0);
        func_001D1EF0();
        func_0018C0D0(D_008101E0, 1);
        D_008106C4[0] = 0;
        (*(unsigned char **)0x70003B6C)[0xB] = 1;
        (*(unsigned char **)0x70003B6C)[0xC] = 0;
        func_001FAE70(1);
        func_001AEE40(0x20);
        return;
    case 6:
        switch (g[0xC]) {
        case 0:
            if (D_00282157[0] == 0) {
                if (D_008106CE[0] == 2) {
                    func_001FF030(D_008106CF[0]);
                } else {
                    func_001FEFE0(D_008106CF[0]);
                }
                g = *(unsigned char **)0x70003B6C;
                g[0xC] = g[0xC] + 1;
            }
            break;
        case 1:
            if (D_00275BD8 == 0) {
                D_008106CE[0] = 0;
                *st = 1;
                (*(unsigned char **)0x70003B6C)[0xC] = 0;
                func_001C1DC0();
                func_001FAE70(1);
            }
            break;
        }
        return;
    }
}
