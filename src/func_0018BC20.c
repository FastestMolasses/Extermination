// NEARMISS func_0018BC20  (vram 0x0018BC20, 0x4A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 9 of 316 instructions differ, in two independent spots. (1) 2 instr: the jtbl reloc pair of ONE of the two dispatchers -- `lui/addiu %hi/%lo(jtbl_0026D950)` vs mwcc's local `%hi/%lo(@62)`. This is the known TWO-TABLE pairing limit, not a missing build/jtblrodata file: I A/B'd it by rebuilding the...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame update for the actor block `p` driven by entity `e`.
// func_00190F20(p, e) always runs first.  The mode byte p[5] selects one of two
// 16-way action dispatchers on the action byte p[6] (any other mode returns):
//   mode 0 -> jtbl_0026D950, mode 1 -> jtbl_0026D910.
// The two tables share most handlers -- 1: func_00197D20, 2: func_00198650,
// 3: func_001936E0, 4 and 7: nothing, 5: func_0018CA90 then force action 7,
// 6: clear the action when the global short D_0028A9A0 is zero, 8:
// func_001914A0 then func_001DD980(D_008105D0, &D_008105E0), 10: func_00198D90
// then func_001D2830(3, 1), 12: func_001963A0, 13: func_00196CE0,
// 15: func_00197390.
// Mode 0 additionally has 9: func_00198CE0, 11: func_00198F10,
// 14: func_00198AF0, and routes action 0 / out-of-range to func_00195130.
// Mode 1 instead routes actions 0, 9, 11, 14 and out-of-range to the main
// locomotion path, which is itself a small state machine on p[1]:
//   p[1] == 0: bump p[1], clear p[2] and the short at p+8, run func_001B0300,
//              then fall through into the p[1] == 1 body; any other value does
//              nothing.
//   p[1] == 1 body: if the action is 0xB and the entity's mode word e+0x230 is
//              not 0x12, clear the action.  Then switch on e+0x230:
//                0x11        -> force action 10 and reset p[1];
//                6,7,8,9,0x2C,0x2D -> nothing;
//                0xA         -> func_0018C6A0(e+0xA0, p+0x20, 0.8) and
//                               func_0018C4B0(p+0x20, 15.0 + e[0xA4], 1.0);
//                5           -> func_0018C6A0(e+0xB0, p+0x20, 0.8) and
//                               func_0018C4B0(p+0x20, e[0xB4] + p[0x8C], 1.0);
//                default     -> same as 0xA.
//              Finally func_0018C0C0(p), func_00193EB0(p, e, 0) and
//              p[0x44] = func_001B1240(D_008105D0, D_008105E0, D_008105E8).

extern void func_00190F20(char *p, char *e);
extern void func_00195130(char *p, char *e);
extern void func_00197D20(char *p, char *e);
extern void func_00198650(char *p, char *e);
extern void func_00198AF0(char *p, char *e);
extern void func_001936E0(char *p, char *e);
extern void func_0018CA90(char *p, char *e);
extern void func_001914A0(char *p, char *e);
extern void func_00198CE0(char *p, char *e);
extern void func_00198D90(char *p, char *e);
extern void func_00198F10(char *p, char *e);
extern void func_001963A0(char *p, char *e);
extern void func_00196CE0(char *p, char *e);
extern void func_00197390(char *p, char *e);
extern void func_001DD980(char *a, float *b);
extern void func_001D2830(int a, int b);
extern void func_001B0300(void);
extern void func_0018C6A0(char *a, char *b, float f);
extern void func_0018C4B0(char *a, float x, float y);
extern void func_0018C0C0(char *a);
extern void func_00193EB0(char *p, char *e, int c);
extern float func_001B1240(char *a, float x, float y);

extern short D_0028A9A0;
extern char D_008105D0[16];
extern float D_008105E0;
extern float D_008105E8;

void func_0018BC20(char *p, char *e) {
    unsigned char u;

    func_00190F20(p, e);
    switch (*(unsigned char *)(p + 5)) {
    case 0:
        switch (*(unsigned char *)(p + 6)) {
        case 0:
        default:
            func_00195130(p, e);
            break;
        case 1:
            func_00197D20(p, e);
            break;
        case 2:
            func_00198650(p, e);
            break;
        case 14:
            func_00198AF0(p, e);
            break;
        case 3:
            func_001936E0(p, e);
            break;
        case 4:
            break;
        case 5:
            func_0018CA90(p, e);
            p[6] = 7;
            break;
        case 6:
            if (D_0028A9A0 == 0) {
                p[6] = 0;
            }
            break;
        case 7:
            break;
        case 8:
            func_001914A0(p, e);
            func_001DD980(D_008105D0, &D_008105E0);
            break;
        case 9:
            func_00198CE0(p, e);
            break;
        case 10:
            func_00198D90(p, e);
            func_001D2830(3, 1);
            break;
        case 11:
            func_00198F10(p, e);
            break;
        case 12:
            func_001963A0(p, e);
            break;
        case 13:
            func_00196CE0(p, e);
            break;
        case 15:
            func_00197390(p, e);
            break;
        }
        break;
    case 1:
        switch (*(unsigned char *)(p + 6)) {
        case 0:
        case 9:
        case 11:
        case 14:
        default:
            u = *(unsigned char *)(p + 1);
            switch (u) {
            case 0:
                *(unsigned char *)(p + 1) = u + 1;
                p[2] = 0;
                *(short *)(p + 8) = 0;
                func_001B0300();
                /* fallthrough */
            case 1:
                if (*(unsigned char *)(p + 6) == 0xB && *(int *)(e + 0x230) != 0x12) {
                    p[6] = 0;
                }
                switch (*(int *)(e + 0x230)) {
                case 0x11:
                    p[6] = 10;
                    p[1] = 0;
                    break;
                case 0x2C:
                case 0x2D:
                case 6:
                case 7:
                case 9:
                case 8:
                    break;
                case 0xA:
                    func_0018C6A0(e + 0xA0, p + 0x20, 0.8f);
                    func_0018C4B0(p + 0x20, 15.0f + *(float *)(e + 0xA4), 1.0f);
                    break;
                case 5:
                    func_0018C6A0(e + 0xB0, p + 0x20, 0.8f);
                    func_0018C4B0(p + 0x20, *(float *)(e + 0xB4) + *(float *)(p + 0x8C), 1.0f);
                    break;
                default:
                    func_0018C6A0(e + 0xA0, p + 0x20, 0.8f);
                    func_0018C4B0(p + 0x20, 15.0f + *(float *)(e + 0xA4), 1.0f);
                    break;
                }
                func_0018C0C0(p);
                func_00193EB0(p, e, 0);
                *(float *)(p + 0x44) = func_001B1240(D_008105D0, D_008105E0, D_008105E8);
                break;
            }
            break;
        case 1:
            func_00197D20(p, e);
            break;
        case 2:
            func_00198650(p, e);
            break;
        case 3:
            func_001936E0(p, e);
            break;
        case 4:
            break;
        case 5:
            func_0018CA90(p, e);
            p[6] = 7;
            break;
        case 6:
            if (D_0028A9A0 == 0) {
                p[6] = 0;
            }
            break;
        case 7:
            break;
        case 8:
            func_001914A0(p, e);
            func_001DD980(D_008105D0, &D_008105E0);
            break;
        case 10:
            func_00198D90(p, e);
            func_001D2830(3, 1);
            break;
        case 12:
            func_001963A0(p, e);
            break;
        case 13:
            func_00196CE0(p, e);
            break;
        case 15:
            func_00197390(p, e);
            break;
        }
        break;
    }
}
