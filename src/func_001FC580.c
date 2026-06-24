// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001FBF50(int arg0, int *p1, int *p2, int p3, float f0, float f1);
extern int D_00281F30;
extern int D_00281F40;
extern int D_00281F50;
extern int D_00281F60;

void func_001FC580(int arg0, int arg1) {
    int *var_s0;
    int sp38;
    int sp3C;

    switch (arg1) {
    case 0x19D:
    case 0x19E:
        var_s0 = &D_00281F30;
        break;
    case 0x19F:
    case 0x1A0:
    case 0x1A1:
        var_s0 = &D_00281F40;
        break;
    case 0x1AA:
    case 0x1B5:
        var_s0 = &D_00281F50;
        break;
    case 0x15A:
    case 0x15B:
        var_s0 = &D_00281F60;
        break;
    default:
        return;
    }

    if (func_001FBF50(arg0, &sp38, &sp3C, 0, 300.0f, 4096.0f) != 0) {
        if (var_s0[0] != 0) {
            if ((sp38 + sp3C) > (var_s0[2] + var_s0[3])) {
                var_s0[1] = arg1;
                var_s0[2] = sp38;
                var_s0[3] = sp3C;
            }
        } else {
            var_s0[0] = 4;
            var_s0[1] = arg1;
            var_s0[2] = sp38;
            var_s0[3] = sp3C;
        }
    }
}
