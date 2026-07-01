// NEARMISS func_00162A40  (vram 0x00162A40, 0x36C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Compound scheduler/register-coloring residual (three independent sites): (1) dispatch-chain beq delay slots for cases 10/3/2/1 -- target leaves them nop but mwcc233 fills with the next case's li immediate (target only fills the first two, cases 12/11); (2) case 11's arg0[0x23F]>=2 guard slti/bnez...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001749A0(unsigned char *e, int id, int z, float f);
extern int func_0017F320(int self);
extern int func_001760C0(void *self, void *pos, int flag, float threshold);
extern void func_00174AC0(unsigned char *p, int n);
extern void func_0017C440(unsigned char *e, int f);
extern int func_00188550(unsigned char *e);
extern void func_001764E0(char *p);
extern void func_0017C540(char *p);
extern void func_001796C0(char *p);
extern int func_00175900(unsigned char *e, int flag);
extern void func_00178B90(char *p, int a);
extern void anim_eval_skeleton(char *p);
extern int func_00102948(void *dst, void *src);
extern int func_001FBD50(unsigned char *e, int b, int c, float f);
extern int D_00275B40;

void func_00162A40(unsigned char *arg0) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x2E0);
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x2E4);
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x2E8);
        *(float *)(arg0 + 0xC4) = *(float *)(arg0 + 0x218);
        if (arg0[0x1F1] == 1) {
            arg0[6] = arg0[6] + 1;
            *(short *)(arg0 + 0x2E) = 0;
            func_001749A0(arg0, 0x7A, 0, 4.0f);
            return;
        }
        arg0[6] = 0xA;
        arg0[0x25F] = 0;
        func_001749A0(arg0, 0x7D, 0, 8.0f);
        func_001764E0((char *)arg0);
        return;
    case 1:
        if (func_0017F320((int)arg0) != 0) {
            arg0[6] = 3;
            return;
        }
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = arg0[6] + 1;
            func_001FBD50(arg0, 0xFF, 0, 300.0f);
            return;
        }
        return;
    case 2:
        if (func_0017F320((int)arg0) != 0) {
            arg0[6] = 3;
            return;
        }
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            arg0[5] = 9;
            arg0[6] = 0;
            arg0[0x1F0] = 0x10;
            arg0[0xD] = 0;
            func_001749A0(arg0, func_00188550(arg0), 0, 16.0f);
            return;
        }
        break;
    case 3:
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        arg0[5] = 7;
        arg0[6] = 0;
        arg0[0x1F0] = 0xD;
        *(float *)(arg0 + 0x2EC) = -0.20000000298023224f;
        return;
    case 10:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            anim_eval_skeleton((char *)arg0);
            func_00102948(arg0 + 0xB0, (void *)(*(int *)(D_00275B40 + 4) + 0xC0));
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 11.0f;
            func_001749A0(arg0, 0x8C, 0, 0.0f);
            if (func_001760C0(arg0, arg0 + 0xB0, 1, 18.0f) != 0) {
                arg0[0x236] = 1;
                arg0[0x235] = arg0[0x235] | 2;
            }
            arg0[6] = arg0[6] + 1;
            return;
        }
        break;
    case 11:
        func_00174AC0(arg0, 0);
        if ((int)arg0[0x23F] >= 2) {
            arg0[6] = arg0[6] + 1;
            func_0017C440(arg0, 1);
        } else {
            arg0[0x25C] = 0;
            func_0017C540((char *)arg0);
            func_001764E0((char *)arg0);
        }
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.2f;
        if (func_00175900(arg0, 1) == 0) {
            *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
            arg0[5] = 7;
            arg0[6] = 0;
            arg0[0x1F0] = 0xD;
            return;
        }
        break;
    case 12:
        func_00178B90((char *)arg0, 1);
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            func_0017C540((char *)arg0);
        }
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -0.2f;
        func_00175900(arg0, 1);
        func_001796C0((char *)arg0);
        break;
    }
}
