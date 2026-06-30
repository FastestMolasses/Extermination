// NEARMISS func_0019AA80  (vram 0x0019AA80, 0x9C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.51% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-delay-slot scheduling: target keeps nop in the beqz delay slot, both 991202 and 233 hoist the following `lui at,0x7000` into it (shifts branch target). Not the clean-store nop class; 233 over-fills here. Scheduling-era wall vs original 2.3.1.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (mwcc 2.3.3 = 98.3%, pinned 991202 = 94.5%). Logic fully recovered.
// Copies 3 floats from arg0[] into the global float array at 0x70003190 and 3
// floats from arg1[] into the parallel array at +0x10, then writes the 1.0f
// constants at 0x700031AC / 0x7000319C, zeroes 0x700031D4, calls
// func_001A7280(arg2 & 0xFFFF) (single arg; the a1/a3 set up in the loop are
// leftovers), and returns 1 (also stored at 0x700031D8) on success, else zeroes
// 0x700031D0 and returns 0. The loop counter and the success flag are zero-init
// via the PS2 paddub idiom, which 2.3.3 reproduces exactly (991202 does not).
//
// WALL: the sole 2.3.3 residual is the beqz delay slot -- the target leaves a
// `nop` there, but both modern mwcc builds hoist the following `lui at,0x7000`
// into the slot (and shift the branch target by 4). Inverting the if-condition
// and adding the explicit `return` did not move it; this is a branch-delay-slot
// scheduling difference vs the original 2.3.1 codegen, not the clean-store nop
// case 2.3.3 fixes. Scheduling-class wall.
extern int func_001A7280(int a);

int func_0019AA80(float *arg0, float *arg1, int arg2) {
    float *dst;
    int i;
    int ret;

    dst = (float *)0x70003190;
    ret = 0;
    i = 0;
    do {
        i += 1;
        *dst = *arg0;
        arg0 += 1;
        *(float *)((char *)dst + 0x10) = *arg1;
        dst += 1;
        arg1 += 1;
    } while (i < 3);
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    *(int *)0x700031D4 = 0;
    if (func_001A7280(arg2 & 0xFFFF) != 0) {
        ret = 1;
    } else {
        *(int *)0x700031D0 = 0;
    }
    *(int *)0x700031D8 = ret;
    return ret;
}
