// NEARMISS func_001FEE60  (vram 0x001FEE60, 0x10C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation permutation: body/control-flow/gp-rel all byte-faithful (96.69% mwcc233); only the s0..s3 coloring of src/dst/counter/&sp[1] differs from the target. permuter-class, parked after several attempts.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Audio/stream table init. Two passes fill parallel 8-byte (lo,hi) records:
// pass 1 reads 2 entries from the gp-rel table D_00275848 into D_0028A480,
// pass 2 reads 0x17 entries from D_00264E40 into D_0028A3C0. Each entry is
// produced by spinning func_00113280(0) / func_00111C28(&buf, src) until
// func_00111C28 returns nonzero, then copying buf[0]/buf[1] out. Finishes
// with func_002032C0() and sub_O_STREAM_MUSIC_DAT_1().
//
// WALL: register-allocation permutation on an otherwise byte-faithful body.
// All residual diffs are ARG_MISMATCH/REPLACE on which s0..s3 register holds
// src (target s2), dst (target s3), the loop counter (target s1) and &sp[1]
// (target s0). Not the clean-store nop; not idiom-fixable -> permuter-class.
extern void func_00113280(int a0);
extern int func_00111C28(int *a0, int a1);
extern void func_002032C0(void);
extern void sub_O_STREAM_MUSIC_DAT_1(void);
extern int D_00275848;
extern int D_00264E40[];
extern int D_0028A480[];
extern int D_0028A3C0[];

void func_001FEE60(void) {
    int sp[8];
    int i;
    int *src;
    int *dst;

    src = &D_00275848;
    dst = D_0028A480;
    i = 0;
    do {
        func_00113280(0);
        while (func_00111C28(&sp[0], *src) == 0) {
            func_00113280(0);
        }
        i = i + 1;
        src = src + 1;
        dst[0] = sp[0];
        dst[1] = sp[1];
        dst = dst + 2;
    } while (i < 2);
    src = D_00264E40;
    dst = D_0028A3C0;
    i = 0;
    do {
        func_00113280(0);
        while (func_00111C28(&sp[0], *src) == 0) {
            func_00113280(0);
        }
        i = i + 1;
        src = src + 1;
        dst[0] = sp[0];
        dst[1] = sp[1];
        dst = dst + 2;
    } while (i < 0x17);
    func_002032C0();
    sub_O_STREAM_MUSIC_DAT_1();
}
