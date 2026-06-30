// NEARMISS func_00164220  (vram 0x00164220, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring permutation at 99.588% (mwcc233). Body and all four state-machine cases are exact and in target order; only register names differ on three sites: (1) the func_001749A0(1.0f,0x74,0) call args colored a0/a1 vs target a1/a2 (the float-in-f12 a0-slot reservation differs b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001749A0(float a, int b, int c);
extern void func_00174AC0(int a);
extern void func_00175900(char *p, int a);
extern void func_001764E0(char *p);
extern void func_00178B90(char *p, int a);
extern void func_001796C0(char *p);
extern void func_0017C440(char *p, int a);
extern void func_0017C540(char *p);
extern float D_0024889C;

void func_00164220(char *p) {
    unsigned char st;
    float t;

    st = *(unsigned char *)(p + 7);
    switch (st) {
    case 0:
        *(unsigned char *)(p + 7) = st + 1;
        func_001749A0(1.0f, 0x74, 0);
        *(float *)(p + 0x38) = 0.8f;
        break;
    case 1:
        if (*(int *)(p + 0x200) & 0x1000) {
            *(unsigned char *)(p + 7) = st + 1;
        } else {
            t = *(float *)(p + 0x38) - D_0024889C;
            *(float *)(p + 0x38) = t;
            if (t <= 0.0f) {
                *(float *)(p + 0x38) = 0.0f;
            }
            func_00178B90(p, 0);
        }
        break;
    case 2:
        func_00174AC0(0);
        if ((int)*(unsigned char *)(p + 0x23F) >= 2) {
            *(unsigned char *)(p + 7) = *(unsigned char *)(p + 7) + 1;
            func_0017C440(p, 0);
        } else {
            *(char *)(p + 0x25C) = 0;
            func_0017C540(p);
        }
        break;
    case 3:
        func_00178B90(p, 0);
        if (!(*(int *)(p + 0x200) & 0x8000)) {
            func_0017C540(p);
        }
        break;
    }
    func_001764E0(p);
    *(float *)(p + 0xB4) = *(float *)(p + 0xB4) + -0.2f;
    func_00175900(p, 1);
    func_001796C0(p);
}
