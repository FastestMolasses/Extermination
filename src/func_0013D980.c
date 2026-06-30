// NEARMISS func_0013D980  (vram 0x0013D980, 0x1E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Prologue register-coloring / scheduling artifact. Entire body is byte-identical; the sole residual is 5 prologue instructions: the target reads the state byte into the caller-saved a2 (`lbu a2,6(a0)`) BEFORE saving self into s1, while mwcc 2.3.3 emits `paddub s1,a0` first and reads the byte into ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Two-state actor tick keyed on the state byte self[6].
// State 0 (spawn): bump the state, set arg1+0x40 = 1.0f, zero arg1+0x48; pick an
// initial counter at arg1+0x20 (1 if the arg1+0x70 short is nonzero, else a
// random table value D_002458E0[(rand>>16)&0xF] via func_00122BB8); zero
// arg1+0x50/+0x4C, then kick clip 9 via anim_clip_init(self, 9, 10.0f, 0.0f).
// State 1 (running): run func_001418F0; if the actor flag self[0xA]&1 is set, or
// (arg1+0x70 nonzero and difficulty D_0081050C >= 2), finish with self[5]=6,
// self[6]=0. Otherwise, when the self+0x2C flags lack bit 0x8000 and the float at
// self+0x3C equals one of {77,64,52,19}, fire func_001FBD50(self,0x805,0,300.0f);
// then if arg1+0x30 has bit 0x1000, decrement the arg1+0x20 counter and either
// re-kick clip 9 (anim_clip_init(self,9,0,0)) while it is nonzero, or finish with
// self[5]=4, self[6]=0 when it reaches zero.
//
// NEARMISS 98.2% (mwcc 2.3.3; 991202 87.8%). Logic fully recovered and the entire
// body is byte-identical. The only residual is the prologue: the target reads the
// state byte into the caller-saved a2 (`lbu a2,6(a0)`) BEFORE saving self into s1,
// while mwcc emits `paddub s1,a0` first and reads the byte into a0 -- a fixed
// register-coloring / prologue-scheduling choice of this compiler version. The 5
// differing instructions are the identical ops with a2<->a0 swapped.
extern void anim_clip_init(int self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_001418F0(void);
extern void func_001FBD50(int self, int a, int b, float c);
extern signed char D_002458E0[16];
extern unsigned char D_0081050C[8];

void func_0013D980(unsigned char *self, unsigned char *arg1) {
    int v;
    unsigned char st = self[6];

    switch (st) {
    case 0:
        self[6] = st + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x48) = 0;
        if (*(short *)(arg1 + 0x70) != 0) {
            *(int *)(arg1 + 0x20) = 1;
        } else {
            *(int *)(arg1 + 0x20) = D_002458E0[(func_00122BB8() >> 16) & 0xF];
        }
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init((int)self, 9, 10.0f, 0.0f);
        break;
    case 1:
        func_001418F0();
        if ((self[0xA] & 1) ||
            (*(short *)(arg1 + 0x70) != 0 && D_0081050C[0] >= 2)) {
            self[5] = 6;
            self[6] = 0;
        } else {
            if ((*(short *)(self + 0x2C) & 0x8000) == 0) {
                float f = *(float *)(self + 0x3C);
                if (f == 77.0f || f == 64.0f || f == 52.0f || f == 19.0f) {
                    func_001FBD50((int)self, 0x805, 0, 300.0f);
                }
            }
            if (*(int *)(arg1 + 0x30) & 0x1000) {
                v = *(int *)(arg1 + 0x20) - 1;
                *(int *)(arg1 + 0x20) = v;
                if (v != 0) {
                    anim_clip_init((int)self, 9, 0.0f, 0.0f);
                } else {
                    self[5] = 4;
                    self[6] = 0;
                }
            }
        }
        break;
    }
}
