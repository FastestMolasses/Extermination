// NEARMISS func_0018ABA0  (vram 0x0018ABA0, 0x3A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-compare delay-slot scheduling: target speculates `addiu a0,s0,0xA0` (else-arm call arg) into the bc1f (c.eq.s) delay slot; mwcc 2.3.3 leaves a nop there. Single-insn instruction-scheduling/permuter-class artifact, not the clean-store nop. Body byte-correct at 99.57%.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 99.57% (mwcc 2.3.3; 991202 = 83.3%). Body and control flow fully
// recovered; sole residual is a single instruction-scheduling artifact (see WALL).
//
// Per-state entity behavior driver, dispatched on the state byte at arg0+4
// (4-way: 0/1, with 2 and 3 sharing the func_001AFC10 path).
// State 0: clears arg0+0xA; if the float at arg0+0xCC is 0 seeds the transform
//   block at arg0+0xA0 (rotation arg0+0xA4 = func_001B1470(arg0+0xA0, pi+D_00810374),
//   scale fields), else copies it from arg0+0xC0 via func_00102948; sets scale
//   arg0+0xAC to 0.0 if arg0+0xD==2 else 1.0; advances state, sets timer fields
//   (0x36=1, 0x28=4), marks arg0+0=1, stores &D_002754E0 into arg0+0x30.
// State 1: if armed (arg0+5==1 && arg0+0xD!=0) emits one of several sound/effect
//   events through func_001EFD90/func_001EFD20/func_001F00A0 keyed on the flag
//   word arg0+0x2E (0x300/0x100/0x10 bits) and, unless arg0+0xD==2, fires
//   func_001FBD50 with a clip id chosen by bit 12 of func_00122BB8(); then bumps
//   arg0+5, decrements the 0x28 timer (state->2 and advance at 0), clears arg0+0xA,
//   and ticks func_001B17A0.
// States 2/3: delegate to func_001AFC10.
//
// Two matching keys baked in: (1) -sdatathreshold 8 makes the gp-range global
// D_002754E0 emit `addiu v1,gp,%gp_rel` (one insn) as the target does; D_00810374
// (out of gp range) is over-declared float[3] so it stays %hi/%lo absolute under
// that threshold. (2) func_001B1470's real signature is (char *p, float a) and
// func_001FBD50's 4th arg is the float 300.0f.
//
// WALL: idx 32 -- the target fills the `bc1f` (c.eq.s on arg0+0xCC) delay slot
// with a speculative `addiu a0,s0,0xA0` (precomputing the else-arm's first call
// arg func_00102948(arg0+0xA0,...)); mwcc 2.3.3 leaves a nop and emits that addiu
// after the branch. Pure delay-slot instruction-scheduling difference across an
// FP compare -- permuter/scheduling class, not the clean-store nop. Inverting the
// branch sense or CSEing arg0+0xA0 both regress. Logic is byte-correct otherwise.
extern void func_00102948(char *a, char *b);
extern int func_00122BB8(void);
extern void func_001AFC10(char *p);
extern float func_001B1470(char *p, float a);
extern void func_001B17A0(char *p);
extern void func_001EFD20(int a, char *b);
extern void func_001EFD90(int a, char *b, char *c);
extern void func_001F00A0(int a, char *b, char *c, int d);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char D_002754E0;
extern float D_00810374[3];

void func_0018ABA0(char *arg0) {
    unsigned char st;
    unsigned short flg;
    short t;

    st = *(unsigned char *)(arg0 + 4);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 0xA) = 0;
        if (*(float *)(arg0 + 0xCC) == 0.0f) {
            *(int *)(arg0 + 0xA0) = 0;
            *(float *)(arg0 + 0xA4) = func_001B1470(arg0 + 0xA0, 3.1415927f + D_00810374[0]);
            *(int *)(arg0 + 0xA8) = 0;
            *(int *)(arg0 + 0xAC) = 0x3F800000;
        } else {
            func_00102948(arg0 + 0xA0, arg0 + 0xC0);
        }
        if (*(unsigned char *)(arg0 + 0xD) == 2) {
            *(int *)(arg0 + 0xAC) = 0;
        } else {
            *(int *)(arg0 + 0xAC) = 0x3F800000;
        }
        *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        *(short *)(arg0 + 0x36) = 1;
        *(short *)(arg0 + 0x28) = 4;
        *(char *)(arg0 + 0) = 1;
        *(char **)(arg0 + 0x30) = &D_002754E0;
        break;
    case 1:
        if (*(unsigned char *)(arg0 + 5) == 1 && *(unsigned char *)(arg0 + 0xD) != 0) {
            if (*(unsigned char *)(arg0 + 0xA) != 0) {
                func_001EFD90(0x80000026, arg0 + 0xB0, arg0 + 0xA0);
            } else {
                flg = *(unsigned short *)(arg0 + 0x2E);
                if ((flg & 0x300) == 0) {
                    func_001EFD20(0x80000019, arg0 + 0xB0);
                    func_001F00A0(0x80000060, arg0 + 0xB0, arg0 + 0xA0, *(unsigned short *)(arg0 + 0x2E) & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if (*(unsigned short *)(arg0 + 0x2E) & 0x10) {
                            if ((func_00122BB8() >> 12) & 1) {
                                func_001FBD50(arg0, 0x18A, 0, 300.0f);
                            } else {
                                func_001FBD50(arg0, 0x18B, 0, 300.0f);
                            }
                        } else if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x188, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x189, 0, 300.0f);
                        }
                    }
                } else if (flg & 0x100) {
                    func_001F00A0(0x80000003, arg0 + 0xB0, arg0 + 0xA0, flg & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x188, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x189, 0, 300.0f);
                        }
                    }
                } else {
                    func_001F00A0(0x80000060, arg0 + 0xB0, arg0 + 0xA0, flg & 1);
                    if (*(unsigned char *)(arg0 + 0xD) != 2) {
                        if ((func_00122BB8() >> 12) & 1) {
                            func_001FBD50(arg0, 0x18A, 0, 300.0f);
                        } else {
                            func_001FBD50(arg0, 0x18B, 0, 300.0f);
                        }
                    }
                }
            }
        }
        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 4) = *(unsigned char *)(arg0 + 4) + 1;
        }
        *(unsigned char *)(arg0 + 0xA) = 0;
        func_001B17A0(arg0);
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
