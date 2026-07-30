// NEARMISS func_001B5360  (vram 0x001B5360, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.56% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 5 residual instructions of 160 (mwcc233 -O4,p -sdatathreshold 0). NOT a jr-table wall: the 13-entry jtbl_0026DEA0 dispatch, its beql-with-lui-in-the-delay-slot out-of-range path, the duplicated default-block head (.L001B5494 lui / .L001B5498 ori) and all eight case bodies match byte-for-byte, jtb...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: builds a vertical probe from an actor (p) and, on a hit, applies a
// correction to the actor's transform block at p+0xB0.  That block is copied
// into the scratchpad vector D_700038A0 (func_00102948 is a 16-byte vector
// copy) and D_700038A0 is copied on to D_700038B0.  D_700038A0.y is then raised
// by 10.0f, and D_700038B0.y is set to D_700038A0.y minus 200.0f for mode 4 and
// minus 30.0f for every other mode -- i.e. the probe reaches much further down
// in mode 4.
// func_0019A570(from, to, 6, 0) is a collision/visibility trace with mask 6; a
// non-zero result means the trace hit, and only then is anything applied.
// On a hit the hit point is taken from the trace result block: D_700038A0 is
// reloaded from D_700031B0 and D_700038B0.x/.y/.z are loaded from +0x24/+0x28/
// +0x2C of the object pointed to by the pointer at 0x700031D0.
//   p[+0x03] = mode (0..12).  A 13-entry jump table picks the blend weight and
//              which of the two appliers runs:
//                0      -> func_001F9100, 2.0f
//                3      -> func_001F9100, 3.5f
//                4      -> func_001F9180, 2.0f
//                5      -> func_001F9100, 5.0f
//                6      -> func_001F9100, 6.5f
//                7      -> func_001F9100, 6.0f
//                8      -> func_001F9180, 8.0f
//                1,2,9..12 and out-of-range -> func_001F9100, 4.2f
//              Both appliers take (p+0xB0, D_700038A0, D_700038B0, weight).

extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700031B0[];

extern void func_00102948(char *, char *);
extern int func_0019A570(char *, char *, int, int);
extern void func_001F9100(char *, char *, char *, float);
extern void func_001F9180(char *, char *, char *, float);

void func_001B5360(unsigned char *p)
{
    char *q;

    func_00102948(D_700038A0, (char *)(p + 0xB0));
    func_00102948(D_700038B0, D_700038A0);
    *(volatile float *)0x700038A4 += 10.0f;
    if (p[3] == 4) {
        *(volatile float *)0x700038B4 = *(volatile float *)0x700038A4 - 200.0f;
    } else {
        *(volatile float *)0x700038B4 = *(volatile float *)0x700038A4 - 30.0f;
    }
    if (func_0019A570(D_700038A0, D_700038B0, 6, 0) != 0) {
        func_00102948(D_700038A0, D_700031B0);
        q = *(char **)0x700031D0;
        *(volatile float *)0x700038B0 = *(float *)(q + 0x24);
        *(volatile float *)0x700038B4 = *(float *)(q + 0x28);
        *(volatile float *)0x700038B8 = *(float *)(q + 0x2C);
        switch (p[3]) {
        case 1:
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
        default:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 4.2f);
            break;
        case 3:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 3.5f);
            break;
        case 0:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 2.0f);
            break;
        case 4:
            func_001F9180((char *)(p + 0xB0), D_700038A0, D_700038B0, 2.0f);
            break;
        case 5:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 5.0f);
            break;
        case 6:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 6.5f);
            break;
        case 7:
            func_001F9100((char *)(p + 0xB0), D_700038A0, D_700038B0, 6.0f);
            break;
        case 8:
            func_001F9180((char *)(p + 0xB0), D_700038A0, D_700038B0, 8.0f);
            break;
        }
    }
}
