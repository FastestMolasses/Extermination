// NEARMISS func_00189EC0  (vram 0x00189EC0, 0x118 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.93% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a single branch target:
// Remaining differences: in the 0xE case the r = 0 arm branches to the if/else join (which then branches to the shared epilogue); mwcc here threads it straight to the epilogue. Every other instruction matches.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Fixed in matching wave 2026-10-27 (was 73.0%): mwcc emits a switch's compare chain in
// REVERSE source case order and its case bodies in source order. The target's compares
// (0x51, 0x30, 0x4F, 0x46, 0x1F, 0x1E, 0x50, 0x1C, 0x6, 0xE) and body layout (0xE, the
// return-1 group, 0x51, default) therefore give the source case order 0xE, then
// 0x6/0x1C/0x50/0x1E/0x1F/0x46/0x4F/0x30, then 0x51, then default, every case ending
// in break with a single return of r.
//
// Maps the type byte at arg0+3 to a small status code:
//   0x51 -> 2
//   0x30 / 0x4F / 0x46 / 0x1F / 0x1E / 0x50 / 0x1C / 0x6 -> 1
//   0xE  -> the "facing angle" case: t = func_0011DF78(func_001B1470(D_00810374 -
//           *(float*)(arg0+0xC4))); stash t at hw scratch 0x70003A20; then
//           func_001000C0(func_00128350(t), pi/2) -> nonzero => 0, zero => 3.
//   default -> 0
// func_00128350 is float-to-double and func_001000C0 a double compare (soft float);
// the extern spelling (int, double) matches the byte-matched twin func_0017D080.
// The pi/2 argument is the float-precision constant 1.5707964f widened to double
// (low word 0x60000000).

extern float D_00810374;
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern int func_00128350(float a);
extern int func_001000C0(int a, double d);

int func_00189EC0(char *arg0) {
    unsigned char k;
    float t;
    int r;

    k = *(unsigned char *)(arg0 + 3);
    switch (k) {
    case 0xE:
        t = func_0011DF78(func_001B1470(D_00810374 - *(float *)(arg0 + 0xC4)));
        *(float *)0x70003A20 = t;
        if (func_001000C0(func_00128350(t), 1.5707964f)) {
            r = 0;
        } else {
            r = 3;
        }
        break;
    case 0x6:
    case 0x1C:
    case 0x50:
    case 0x1E:
    case 0x1F:
    case 0x46:
    case 0x4F:
    case 0x30:
        r = 1;
        break;
    case 0x51:
        r = 2;
        break;
    default:
        r = 0;
        break;
    }
    return r;
}
