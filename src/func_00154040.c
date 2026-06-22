// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 1
//
// Actor/entity spawn-init. Guarded by func_001B10B0(arg0, 0x14, 0x13) == 0
// (resource/slot reservation; returns 0 on success). On success: runs
// bone_init_default_2(arg0, 1) and func_001CA5F0(arg0, 9), then stores the
// spec id D_0028A4D8 at +0x58 (loaded once, reused as the 2nd arg to
// func_001B1240). Writes out-param *arg1 = 1; state bytes +4 = +0 = 1;
// behavior/table ptr +0x30 = &D_00275448; +0x34 = 0xA. Computes +0xC4 via
// func_001B1240(arg0+0xB0, spec, D_00810350, D_00810358) (a heading/scale).
// Seeds float fields +0x80=0.5, +0x84=0.875, +0x88=+0x8C=1.0, then calls
// func_001FBD50(arg0, 0x430, 0, 300.0f).
//
// -sdatathreshold 1: D_00275448 (a 1-byte global) is gp-relative in the
// target while the 4-byte int/float globals are %hi/%lo; threshold 1 selects
// exactly that split. Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the
// pinned 991202. objdiff 100% vs build/expected/func_00154040.o.
extern int func_001B10B0(char *, int, int);
extern void bone_init_default_2(char *, int);
extern void func_001CA5F0(char *, int);
extern float func_001B1240(char *, int, float, float);
extern void func_001FBD50(char *, int, int, float);
extern char D_00275448;
extern int D_0028A4D8;
extern float D_00810350;
extern float D_00810358;

void func_00154040(char *arg0, short *arg1) {
    int spec;

    if (func_001B10B0(arg0, 0x14, 0x13) == 0) {
        bone_init_default_2(arg0, 1);
        func_001CA5F0(arg0, 9);
        spec = D_0028A4D8;
        *(int *)(arg0 + 0x58) = spec;
        *arg1 = 1;
        *(char *)(arg0 + 4) = 1;
        *(char *)(arg0 + 0) = 1;
        *(int *)(arg0 + 0x30) = (int)&D_00275448;
        *(short *)(arg0 + 0x34) = 0xA;
        *(float *)(arg0 + 0xC4) = func_001B1240(arg0 + 0xB0, spec, D_00810350, D_00810358);
        *(int *)(arg0 + 0x80) = 0x3F000000;
        *(int *)(arg0 + 0x84) = 0x3F600000;
        *(int *)(arg0 + 0x88) = 0x3F800000;
        *(int *)(arg0 + 0x8C) = 0x3F800000;
        func_001FBD50(arg0, 0x430, 0, 300.0f);
    }
}
