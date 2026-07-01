// NEARMISS func_001C1F50  (vram 0x001C1F50, 0x344 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.13% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure/constants/call-arg-order fully recovered: two independent linear dispatch chains over the same packed 16-bit level key, including a 64-bit bit-packer idiom for func_001E2260/func_001E2280 GS-register writer args. An 8-value fallthrough-case group is best as a switch (an ||-conditio...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Level-transition sound/event dispatcher, keyed on the packed 16-bit level id
// (D_00810700<<8 | D_00810701). Two independent linear if-else chains against the
// SAME key (not a jump table):
//  1) func_001D2830(0x20,flag)+func_001D2830(0x21,flag)[+more] to arm/disarm a
//     couple of subsystem flags: level 0x1500 arms all four (0x20/21/22/23=1);
//     levels 0x1100/0xF01/0xF00/0xE00/0xD00/0xC00/0xB00/0x1200 arm 0x20/0x21=1 and
//     clear 0x22; anything else clears 0x20 only.
//  2) per-level a 64-bit packed constant is passed to func_001E2260 (a GS-register
//     writer) for levels 0xB00/0xD00/0xE00/0xF00/0xF01/0x1100/0x1200/0x1500; other
//     levels skip the call entirely.
// Then unconditionally func_001E2270(&D_00250F30); then, if the level is 0x1500,
// func_001E2280 with another 64-bit packed constant; then, if the level is
// 0x1200 or 0xF00, func_001D2830(0x25,1).
extern void func_001D2830(int a, int b);
extern void func_001E2260(long long packed);
extern void func_001E2270(void *p);
extern void func_001E2280(long long packed);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_00250F30;

void func_001C1F50(void) {
    int level;

    level = (D_00810700 << 8) + D_00810701;
    switch (level) {
    case 0x1500:
        func_001D2830(0x20, 1);
        func_001D2830(0x21, 1);
        func_001D2830(0x22, 1);
        func_001D2830(0x23, 1);
        break;
    case 0x1200:
    case 0x1100:
    case 0xF01:
    case 0xF00:
    case 0xE00:
    case 0xD00:
    case 0xC00:
    case 0xB00:
        func_001D2830(0x20, 1);
        func_001D2830(0x21, 1);
        func_001D2830(0x22, 0);
        break;
    default:
        func_001D2830(0x20, 0);
        break;
    }

    level = (D_00810700 << 8) + D_00810701;
    if (level == 0xB00) {
        func_001E2260(0x21323200 | (0x20069F01LL << 0x20));
    } else if (level == 0xD00) {
        func_001E2260(0x21323240 | (0x2006D501LL << 0x20));
    } else if (level == 0xE00) {
        func_001E2260(0x21323440 | (0x2006EA81LL << 0x20));
    } else if (level == 0xF00) {
        func_001E2260(0x21322F82 | (0x20065901LL << 0x20));
    } else if (level == 0xF01) {
        func_001E2260(0x21323280 | (0x2006B481LL << 0x20));
    } else if (level == 0x1100) {
        func_001E2260(0x213230C0 | (0x20066001LL << 0x20));
    } else if (level == 0x1200) {
        func_001E2260(0x21323300 | (0x2006CC81LL << 0x20));
    } else if (level == 0x1500) {
        func_001E2260(0x21323700 | (0x20076A81LL << 0x20));
    }

    func_001E2270(&D_00250F30);

    level = (D_00810700 << 8) + D_00810701;
    if (level == 0x1500) {
        func_001E2280(0xDD321D00LL | (0x20042B05LL << 0x20));
    }

    level = (D_00810700 << 8) + D_00810701;
    if (level == 0x1200 || level == 0xF00) {
        func_001D2830(0x25, 1);
    }
}
