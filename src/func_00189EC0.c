// NEARMISS func_00189EC0  (vram 0x00189EC0, 0x118 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// switch case-body layout / branch-lowering permutation: target places the return-1/return-2 bodies AFTER the 0xE float body with a split epilogue (lq ra duplicated). Compares are in source order but body layout is mwcc's own; not reproducible by reordering source cases (cascade form scores 22%). F...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (o233 73.0%, o991 60.2%): logic fully recovered; sole residual is a
// switch case-body layout / branch-lowering artifact.
//
// Maps the type byte at arg0+3 to a small status code:
//   0x51 -> 2
//   0x30 / 0x4F / 0x46 / 0x1F / 0x1E / 0x50 / 0x1C / 0x6 -> 1
//   0xE  -> the "facing angle" case: t = func_0011DF78(func_001B1470(D_00810374 -
//           *(float*)(arg0+0xC4))); stash t at hw scratch 0x70003A20; then
//           func_001000C0(func_00128350(t), pi/2) -> nonzero => 0, zero => 3.
//   default -> 0
// The pi/2 argument is the float-precision constant 1.5707964f widened to double
// (0x3FF921FB60000000), which is what reproduces the target's single `lui 0x6000`
// (no ori) low word -- the full double pi/2 would emit lui 0x5444 / ori 0x2d18.
//
// WALL: in the target, mwcc places the return-1 and return-2 case bodies AFTER the
// 0xE float body and uses a split epilogue (one `lq ra` in the 0xE-path branch slot,
// one shared at the tail). The compares stay in source order but the body layout is
// mwcc's own ordering, which neither build reproduces from this (or a cascade)
// source shape. A branch-lowering/case-layout permutation; all values + math match.

extern float D_00810374;
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern int func_00128350(float a);
extern int func_001000C0(int a, double d);

int func_00189EC0(char *arg0) {
    unsigned char k;
    int r;

    k = *(unsigned char *)(arg0 + 3);
    switch (k) {
    case 0x51:
        r = 2;
        break;
    case 0x30:
        r = 1;
        break;
    case 0x4F:
    case 0x46:
    case 0x1F:
    case 0x1E:
    case 0x50:
    case 0x1C:
    case 0x6:
        r = 1;
        break;
    case 0xE: {
        float t;
        t = func_0011DF78(func_001B1470(D_00810374 - *(float *)(arg0 + 0xC4)));
        *(float *)0x70003A20 = t;
        if (func_001000C0(func_00128350(t), 1.5707964f)) {
            r = 0;
        } else {
            r = 3;
        }
        break;
    }
    default:
        r = 0;
        break;
    }
    return r;
}
