// NEARMISS func_00167C80  (vram 0x00167C80, 0x3C4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// State-machine dispatch (switch on arg0[6], ascending case labels 0-3 with nested sub-switches for case 2) is correctly lowered by mwcc into the target's exact reversed compare-chain (idiom 14), but two small branch-likely/switch-lowering scheduling residuals remain: (1) case 0's beqzl delay slot ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_00182AB0(void *obj);
extern void func_00182430(void *obj, int a, int b);
extern void func_001FBD50(void *p, int id, int flags, float vol);
extern float func_001B1470(float a);
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *dst, int a, void *c);
extern void func_00102918(void *dst, void *a, void *b);
extern void func_0017FF80(void *p, float a);
extern char *D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];

void func_00167C80(unsigned char *arg0) {
    unsigned char st;
    unsigned char sub;
    void *a0;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg0[7] = 0;
        if (arg0[0x1F0] == 0x1B) {
            func_001749A0(arg0, 0x94, 0, 8.0f);
            return;
        }
        func_001749A0(arg0, 0x95, 0, 8.0f);
        return;
    case 1:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            arg0[6] = st + 1;
        }
        return;
    case 2:
        if (arg0[0x1F0] == 0x1B) {
            sub = arg0[7];
            switch (sub) {
            case 0:
                if (*(float *)(arg0 + 0x3C) <= 24.0f) {
                    arg0[7] = sub + 1;
                    func_00182AB0(arg0);
                }
                return;
            case 1:
                if (*(float *)(arg0 + 0x3C) <= 5.0f) {
                    arg0[6] = st + 1;
                    func_00182AB0(arg0);
                    func_001FBD50(arg0, 0x119, 0, 300.0f);
                }
                return;
            }
            return;
        }
        sub = arg0[7];
        switch (sub) {
        case 0:
            if (*(float *)(arg0 + 0x3C) <= 99.0f) {
                arg0[7] = sub + 1;
                func_00182430(arg0, 2, sub);
            }
            return;
        case 1:
            if (*(float *)(arg0 + 0x3C) <= 69.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            return;
        case 2:
            if (*(float *)(arg0 + 0x3C) <= 48.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            return;
        case 3:
            if (*(float *)(arg0 + 0x3C) <= 23.0f) {
                arg0[7] = sub + 1;
                func_00182AB0(arg0);
            }
            return;
        case 4:
            if (*(float *)(arg0 + 0x3C) <= 2.0f) {
                arg0[6] = st + 1;
                func_00182AB0(arg0);
                func_001FBD50(arg0, 0x119, 0, 300.0f);
            }
            return;
        }
        return;
    case 3:
        if (!(*(int *)(arg0 + 0x200) & 0x1000)) {
            return;
        }
        a0 = arg0 + 0xD0;
        if (arg0[0x1F0] == 0x1C) {
            *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
            a0 = arg0 + 0xD0;
        }
        func_001029C0(a0);
        func_00102C58(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xC0);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0xC1266666;
        *(int *)0x700038A8 = 0x4089999A;
        *(int *)0x700038AC = 0;
        func_001026A0(D_700038B0, arg0 + 0xD0, D_700038A0);
        func_001028B8(arg0 + 0xB0, *(int *)(D_00275B40 + 4) + 0xC0, D_700038B0);
        *(float *)(arg0 + 0xBC) = 1.0f;
        func_00102918(arg0 + 0xD0, arg0 + 0xD0, arg0 + 0xB0);
        arg0[0x2F1] = 0;
        func_0017FF80(arg0, 0.0f);
        arg0[5] = 0xE;
        arg0[6] = 0;
        arg0[0x1F0] = 0x1D;
        return;
    }
}
