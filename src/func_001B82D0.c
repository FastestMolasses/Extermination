// NEARMISS func_001B82D0  (vram 0x001B82D0, 0x7DC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 25 residual instructions out of 506; the jtbl_0026DFE0 dispatch, the case-5 -> case-4 fallthrough layout and every reloc match. (1) 17 instrs, 7 sites: conditional-branch delay-slot speculation. At `beqz v1,<0x70003B92 block>`, `bne a0,v1,<0x70003B8D store>`, `beqz v1,<0x70003B92 store>` (x2), `b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: Cut-scene / event-message handler. `ev` is an event record; the
// opcode is the word at ev+8 and is dispatched through jtbl_0026DFE0 (14 slots).
// `st` is the per-handler state block: st[4] is the phase counter, st[0xC] a
// "cinematic active" flag. Returns 0 to keep running, 1 to yield this frame and
// 3 when the cinematic tears down.
//   op 0/1/13  - fade-in phase: arms D_008101E4, sets the scratch-pad mode
//                byte 0x70003B8D (1 for op 13, else 2), clears the frame
//                counters 0x70003B84/0x70003B91 and D_008106F3.
//   op 2/3/7/8 - same, plus func_001AEB60(4)/func_001BA510(); phase 1 raises
//                0x70003B92 / st[0xC] and optionally func_001B81D0(D_008102B0).
//   op 9..12   - 5-phase sub-state machine (audio via func_001AEDE0/func_001FD4C0
//                /func_00119828, gate on the short D_0028A9A0, flag D_008106F4).
//   op 5       - marks D_00810758[ev->0x14] = 0xFF then FALLS THROUGH into op 4.
//   op 4       - cinematic teardown: resets the HUD bytes D_008101E1..E6, sets
//                D_008106EF = 0x50, func_001AEBA0(4), D_002821B4 = 2, optional
//                func_001CA770(D_008102B0), func_001D25F0(480.0f), rewrites the
//                camera vector D_008105F0..FC to (0,-1,0,1) and returns 3 when
//                both 0x70003B91 and st[0xC] are 2 (else 1).
//   op 6       - records D_008107D8[ev->0x14] = ev->0x18 then the same teardown.
// arg0 is unused by the body (the caller's context pointer).

extern unsigned char D_008101E1;
extern unsigned char D_008101E2;
extern unsigned char D_008101E3;
extern unsigned char D_008101E4;
extern unsigned char D_008101E6;
extern unsigned char D_008106EF;
extern unsigned char D_008106F3;
extern unsigned char D_008106F4;
extern unsigned char D_00810758[];
extern unsigned char D_008107D8[];
extern char D_008102B0[];
extern short D_0028A9A0;
extern int D_002821B4;
extern volatile float D_008105F0;
extern volatile float D_008105F4;
extern volatile float D_008105F8;
extern volatile float D_008105FC;

extern void func_00119828(int a0, int a1, int a2);
extern void func_001AEB60(short a0);
extern void func_001AEBA0(short a0);
extern void func_001AEDE0(short a0, unsigned char a1);
extern void func_001AEE10(short a0, unsigned char a1);
extern void func_001B81D0(int arg0);
extern void func_001BA510(void);
extern void func_001CA770(char *a0);
extern void func_001D25F0(float fa0);
extern void func_001D2610(float arg0);
extern int func_001FD4C0(int arg0);
extern void func_001FAE70(int arg0);

int func_001B82D0(int arg0, unsigned char *st, unsigned char *ev) {
    int op;
    int mode;

    op = *(int *)(ev + 8);
    switch (op) {
    case 0:
    case 1:
    case 13:
        switch (st[4]) {
        case 0:
            if (*(unsigned char *)0x70003B92) {
                return 1;
            }
            D_008101E4 = 2;
            if (*(int *)(ev + 8) == 0xD) {
                *(unsigned char *)0x70003B8D = 1;
            } else {
                *(unsigned char *)0x70003B8D = 2;
            }
            st[4] = st[4] + 1;
            *(unsigned short *)0x70003B84 = 0;
            *(unsigned char *)0x70003B91 = 0;
            D_008106F3 = 0;
            break;
        case 1:
            if (*(int *)(ev + 0x14)) {
                *(unsigned char *)0x70003B92 = 1;
                st[0xC] = 1;
                if (*(int *)(ev + 8) == 1) {
                    *(unsigned char *)0x70003B91 = 1;
                }
                func_001D2610(0.0f);
                return 1;
            } else {
                if (*(unsigned char *)0x70003B8F == 0) {
                    break;
                }
                *(unsigned char *)0x70003B92 = 1;
                st[0xC] = 1;
                if (*(int *)(ev + 8) == 1) {
                    *(unsigned char *)0x70003B91 = 1;
                }
                func_001D2610(0.0f);
                return 1;
            }
        }
        break;
    case 2:
    case 3:
    case 7:
    case 8:
        switch (st[4]) {
        case 0:
            if (*(unsigned char *)0x70003B92) {
                return 1;
            }
            D_008101E4 = 1;
            *(unsigned char *)0x70003B8D = 2;
            func_001AEB60(4);
            st[4] = st[4] + 1;
            *(unsigned short *)0x70003B84 = 0;
            *(unsigned char *)0x70003B91 = 0;
            func_001BA510();
            D_008106F3 = 0;
            break;
        case 1:
            if (*(int *)(ev + 0x14)) {
                *(unsigned char *)0x70003B92 = 1;
                st[0xC] = 1;
                if (*(int *)(ev + 8) == 3 || *(int *)(ev + 8) == 8) {
                    *(unsigned char *)0x70003B91 = 1;
                }
                if (*(int *)(ev + 8) == 7 || *(int *)(ev + 8) == 8) {
                    func_001B81D0((int)D_008102B0);
                }
                func_001D2610(0.0f);
                return 1;
            } else {
                if (*(unsigned char *)0x70003B8F == 0) {
                    break;
                }
                *(unsigned char *)0x70003B92 = 1;
                st[0xC] = 1;
                if (*(int *)(ev + 8) == 3 || *(int *)(ev + 8) == 8) {
                    *(unsigned char *)0x70003B91 = 1;
                }
                if (*(int *)(ev + 8) == 7 || *(int *)(ev + 8) == 8) {
                    func_001B81D0((int)D_008102B0);
                }
                func_001D2610(0.0f);
                return 1;
            }
        }
        break;
    case 9:
    case 10:
    case 11:
    case 12:
        switch (st[4]) {
        case 0:
            if (*(unsigned char *)0x70003B92) {
                return 1;
            }
            D_008101E4 = 1;
            *(unsigned char *)0x70003B8D = 2;
            *(unsigned short *)0x70003B84 = 0;
            *(unsigned char *)0x70003B91 = 0;
            func_001BA510();
            D_008106F3 = 0;
            func_001AEDE0(4, 0);
            func_001FD4C0(*(int *)(ev + 0x18));
            st[4] = st[4] + 1;
            func_00119828(0, 0, 0);
            func_00119828(1, 0, 0);
            break;
        case 1:
            if (D_0028A9A0 == 2) {
                func_001AEB60(0xFF);
                st[4] = st[4] + 1;
            }
            break;
        case 2:
            if (*(int *)(ev + 0x14)) {
                if (op == 11 || op == 12) {
                    func_001B81D0((int)D_008102B0);
                }
                st[4] = st[4] + 1;
                func_001D2610(0.0f);
            } else {
                if (*(unsigned char *)0x70003B8F == 0) {
                    break;
                }
                if (op == 11 || op == 12) {
                    func_001B81D0((int)D_008102B0);
                }
                func_001D2610(0.0f);
                st[4] = st[4] + 1;
            }
            break;
        case 3:
            if (D_008106F4 != 1) {
                break;
            }
            func_001AEE10(0x10, 0);
            *(unsigned char *)0x70003B92 = 1;
            st[0xC] = 1;
            if (*(int *)(ev + 8) == 0xA || *(int *)(ev + 8) == 0xC) {
                *(unsigned char *)0x70003B91 = 1;
            }
            return 1;
        case 4:
            if (D_0028A9A0 != 0) {
                break;
            }
            *(unsigned char *)0x70003B92 = 1;
            st[0xC] = 1;
            if (*(int *)(ev + 8) == 0xA || *(int *)(ev + 8) == 0xC) {
                *(unsigned char *)0x70003B91 = 1;
            }
            return 1;
        }
        break;
    case 5:
        D_00810758[*(int *)(ev + 0x14)] = 0xFF;
        /* fallthrough */
    case 4:
        if (*(unsigned char *)0x70003B8D == 0) {
            return 1;
        }
        D_008106F3 = 0;
        D_008101E4 = 0;
        D_008106EF = 0x50;
        if (D_008101E6 == 3) {
            D_008101E6 = 0;
            D_008101E1 = 0;
            D_008101E2 = 0;
            D_008101E3 = 0;
        } else {
            D_008101E1 = 0;
        }
        func_001AEBA0(4);
        mode = *(unsigned char *)0x70003B8F;
        D_002821B4 = 2;
        if (mode == 2) {
            func_001CA770(D_008102B0);
            *(unsigned char *)0x70003B8F = 1;
        }
        func_001D25F0(480.0f);
        D_008105F0 = 0.0f;
        D_008105F4 = -1.0f;
        D_008105F8 = 0.0f;
        D_008105FC = 1.0f;
        if (*(unsigned char *)0x70003B91 == 2 && *(char *)(st + 0xC) == 2) {
            func_001FAE70(0);
            func_001AEE10(4, 0);
            st[0xC] = 0;
            *(unsigned char *)0x70003B8D = 0;
            *(unsigned char *)0x70003B92 = 0;
            *(unsigned char *)0x70003B91 = 0;
            return 3;
        }
        st[0xC] = 0;
        *(unsigned char *)0x70003B8D = 0;
        *(unsigned char *)0x70003B92 = 0;
        *(unsigned char *)0x70003B91 = 0;
        return 1;
    case 6:
        D_008107D8[*(int *)(ev + 0x14)] = *(unsigned char *)(ev + 0x18);
        if (*(unsigned char *)0x70003B8D == 0) {
            return 1;
        }
        D_008106F3 = 0;
        D_008101E4 = 0;
        D_008101E1 = 0;
        D_008106EF = 0x50;
        func_001AEBA0(4);
        mode = *(unsigned char *)0x70003B8F;
        D_002821B4 = 2;
        if (mode == 2) {
            func_001CA770(D_008102B0);
            *(unsigned char *)0x70003B8F = 1;
        }
        func_001D25F0(480.0f);
        D_008105F0 = 0.0f;
        D_008105F4 = -1.0f;
        D_008105F8 = 0.0f;
        D_008105FC = 1.0f;
        if (*(unsigned char *)0x70003B91 == 2 && *(char *)(st + 0xC) == 2) {
            func_001FAE70(0);
            func_001AEE10(4, 0);
            st[0xC] = 0;
            *(unsigned char *)0x70003B8D = 0;
            *(unsigned char *)0x70003B92 = 0;
            *(unsigned char *)0x70003B91 = 0;
            return 3;
        }
        st[0xC] = 0;
        *(unsigned char *)0x70003B8D = 0;
        *(unsigned char *)0x70003B92 = 0;
        *(unsigned char *)0x70003B91 = 0;
        return 1;
    }
    return 0;
}
