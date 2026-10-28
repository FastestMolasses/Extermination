// NEARMISS func_001D5C80  (vram 0x001D5C80, 0x728 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); matching wave 2026-10-28.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Remaining differences:
// after each pair of clip-flag reads, and after the matrix reload, the original has
// integer nops (eight and two) that only a non-VU0 inline asm could produce (not
// approved); saved-register assignment and the far-plane constant setup order differ.
// Fixes so far: the AABB clip runs as VU0 asm (corner lanes y, z, w from the two
// quadwords at obj + 0x10); the any-flag test is the low 24 bits of the OR of the
// two reads (a 64-bit shift pair); clamps written "> 0x1F"; z0 doubles as the row
// counter.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Static-object sweep around pos: picks the grid cells within +-15 units (at least
// the 3x3 around the centre cell, clamped to 32x32), rebuilds the screen and
// guard-band clip matrices in scratchpad, and for every object in each cell clip-tests
// its AABB: wholly outside one plane -> skip; wholly inside -> func_001D4FB0; else
// retest against the guard band and, if it still crosses, send it through the clip
// path (func_001D1F80(0, 2, 6), func_001D4B50, func_001D4CD0).

extern char *D_00275670;
extern float D_00810610[16];
extern char *D_0028A5A0[];
extern float D_70003400[16];
extern float D_70003440[16];

extern int  float_to_int(float v);
extern char *func_001C6120(char *bank, int id);
extern void func_001D2D20(float *m, float focal, float w, float h,
                          float near, float far);
extern void func_001026D0(float *dst, float *m, float *v);
extern void func_001D4CD0(void);
extern void func_001D4FB0(char *obj);
extern void func_001D4B50(char *obj);
extern void func_001D1F80(int chan, int a, int b);
extern void func_001D1FF0(int chan, int a);

void func_001D5C80(float *pos)
{
    char *ctx;
    int *grid;
    int stride;
    float cellX, cellZ, origX, origZ;
    int cx, cz;
    int x0, x1, z0, z1;
    int ix, slot;
    float *m;

    ctx    = D_00275670;
    grid   = *(int **)(ctx + 0x140);
    stride = *(int *)(ctx + 0x148);
    cellX  = *(float *)(ctx + 0x150);
    cellZ  = *(float *)(ctx + 0x154);
    origX  = *(float *)(ctx + 0x158);
    origZ  = *(float *)(ctx + 0x15C);

    cx = float_to_int(1.0f + (pos[0] - origX) / cellX);
    cz = float_to_int(1.0f + (pos[2] - origZ) / cellZ);
    x0 = float_to_int(((pos[0] - origX) - 15.0f) / cellX - 0.5f);
    x1 = float_to_int(0.5f + (15.0f + (pos[0] - origX)) / cellX);
    z0 = float_to_int(((pos[2] - origZ) - 15.0f) / cellZ - 0.5f);
    z1 = float_to_int(0.5f + (15.0f + (pos[2] - origZ)) / cellZ);

    if (x0 >= cx - 1) x0 = cx - 1;
    if (x1 <= cx + 1) x1 = cx + 1;
    if (z0 >= cz - 1) z0 = cz - 1;
    if (z1 <= cz + 1) z1 = cz + 1;

    if (x0 < 0)       x0 = 0;
    if (x1 > 0x1F)    x1 = 0x1F;
    if (z0 < 0)       z0 = 0;
    if (z1 > 0x1F)    z1 = 0x1F;

    func_001D4CD0();
    func_001D1FF0(0, 0);

    func_001D2D20(D_70003440, *(float *)(D_00275670 + 0x2468), 4096.0f, 4096.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003440, D_70003440, D_00810610);
    func_001D2D20(D_70003400, *(float *)(D_00275670 + 0x2468), 1024.0f, 448.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003400, D_70003400, D_00810610);

    m = D_70003400;
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));

    for (; z0 <= z1; z0++) {
        for (ix = x0; ix <= x1; ix++) {
            for (slot = 0; slot < 4; slot++) {
                char *obj;
                char *box;
                int lo4, hi4;
                unsigned int v;
                unsigned long long any;
                int id;

                id = grid[(z0 + ix * stride) * 4 + slot];
                if (id <= 0) {
                    continue;
                }
                obj = func_001C6120(D_0028A5A0[0], id);
                box = obj + 0x10;
                asm("lqc2 vf1, 0x0(%0)" : : "r"(box));
                asm("lqc2 vf2, 0x10(%0)" : : "r"(box));
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("cfc2.ni %0, vi18" : "=r"(lo4));
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("cfc2.ni %0, vi18" : "=r"(hi4));

                any = (unsigned long long)(lo4 | hi4) << 40 >> 40;
                v = (unsigned int)lo4 & hi4;
                v = (v & (v >> 6)) & ((v >> 12) & (v >> 18));
                v &= 0x3F;
                if ((v ^ (v << 1)) & 0x2A) {
                    continue;
                }
                if (any == 0) {
                    func_001D4FB0(obj);
                    continue;
                }
                m = D_70003440;
                asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
                asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
                asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
                asm("lqc2 vf27, 0x30(%0)" : : "r"(m));

                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf1w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("cfc2.ni %0, vi18" : "=r"(lo4));
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf1z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf1y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vmulay.xyzw ACC, vf24, vf2y");
                asm("vmaddaz.xyzw ACC, vf25, vf2z");
                asm("vmaddaw.xyzw ACC, vf26, vf2w");
                asm("vmaddw.xyzw vf3, vf27, vf0w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vclipw.xyz vf3, vf3w");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("vnop");
                asm("cfc2.ni %0, vi18" : "=r"(hi4));

                if (((unsigned long long)(lo4 | hi4) << 40 >> 40) != 0) {
                    func_001D4FB0(obj);
                    func_001D1F80(0, 2, 6);
                    func_001D4B50(obj);
                    func_001D4CD0();
                } else {
                    func_001D4FB0(obj);
                }
                m = D_70003400;
                asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
                asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
                asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
                asm("lqc2 vf27, 0x30(%0)" : : "r"(m));

            }
        }
    }
    func_001D1FF0(0, 1);
}
