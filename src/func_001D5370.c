// NEARMISS func_001D5370  (vram 0x001D5370, 0x700 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.76% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 39.18%; matching wave 2026-10-28).
// Remaining differences: after each of the four clip-flag reads that end an 8-corner run
// the original has eight integer no-ops (not VU0 instructions; with them added as asm the
// candidate scores 96.33), and the saved-register colouring of the loop variables differs
// (the original: grid s2, stride s3, x1 s4, z s5, x s0, slot s1, x0 spilled to the stack).
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Fixes found: the stage pick is a switch (0x0D00 written before 0x1300) and the room pick
// a switch (case 4, then 5/9/7/8 sharing one body); the dynamic-object stage switch lists
// its cases in ascending order; the stage bytes are not gp-relative; the AABB corners are
// built from the y, z, w lanes of the qwords at obj + 0x10 / + 0x20 (min x,y,z at +0x14,
// max at +0x24).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Static map-object pass: sweeps the 32x32 object grid (some stages a sub-rectangle),
// frustum-tests each object's AABB with VU0 clip flags against the screen matrix, rejects
// boxes wholly outside one plane, draws boxes wholly inside with func_001D4FB0 and retests
// the rest against the guard-band matrix (software clip path func_001D4B20 if they leave
// it), then runs the dynamic-object pass for the listed stages.

extern char *D_00275670;
extern float D_00810610[16];
extern unsigned char D_00810700[];
extern char *D_0028A5A0[];

extern float D_70003400[16];
extern float D_70003440[16];

extern char *func_001C6120(char *bank, int id);
extern void func_001D2D20(float *m, float focal, float w, float h,
                          float near, float far);
extern void func_001026D0(float *dst, float *m, float *v);
extern void func_001D4DA0(void);
extern void func_001D4FB0(char *obj);
extern void func_001D4B20(char *obj);
extern void func_001D1F80(int chan, int a, int b);
extern void func_001D5BD0(void);

void func_001D5370(void)
{
    int *grid;
    int stride;
    int stage;
    int x0, x1, z0, z1;
    int ix, iz, slot;
    char *obj;
    float *m;
    float *box;
    unsigned int lo, hi, all, v;
    int id;

    grid   = *(int **)(D_00275670 + 0x140);
    stride = *(int *)(D_00275670 + 0x148);
    stage = (D_00810700[0] << 8) + D_00810700[1];

    x0 = 0;
    x1 = 0x20;
    z0 = 0;
    z1 = x1;

    switch (stage) {
    case 0x0D00:
        if (D_00810700[2] < 4) {
            x0 = 0x14;
            x1 = 0x17;
            z0 = 0x0F;
            z1 = x1;
        }
        break;
    case 0x1300:
        switch (D_00810700[2]) {
        case 4:
            x0 = 9;
            z1 = 0x19;
            break;
        case 5:
        case 9:
        case 7:
        case 8:
            x0 = 0;
            x1 = 0x1E;
            z0 = 0x0C;
            z1 = 0x20;
            break;
        }
        break;
    }

    func_001D4DA0();

    func_001D2D20(D_70003440, *(float *)(D_00275670 + 0x2468),
                  4096.0f, 4096.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003440, D_70003440, D_00810610);
    func_001D2D20(D_70003400, *(float *)(D_00275670 + 0x2468),
                  1024.0f, 448.0f, 0.1f, 16711680.0f);
    func_001026D0(D_70003400, D_70003400, D_00810610);

    m = D_70003400;
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    for (iz = z0; iz < z1; iz++) {
        for (ix = x0; ix < x1; ix++) {
            for (slot = 0; slot < 4; slot++) {
                id = grid[(ix * stride + iz) * 4 + slot];
                if (id <= 0) {
                    continue;
                }
                obj = func_001C6120(D_0028A5A0[0], id);
                box = (float *)(obj + 0x10);
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
                asm("cfc2.ni %0, vi18" : "=r"(lo));
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
                asm("cfc2.ni %0, vi18" : "=r"(hi));
                all = (lo | hi) & 0xFFFFFF;
                v = lo & hi;
                v = (v & (v >> 6)) & ((v >> 12) & (v >> 18));
                v &= 0x3F;
                if ((v ^ (v << 1)) & 0x2A) {
                    continue;
                }
                if (all == 0) {
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
                asm("cfc2.ni %0, vi18" : "=r"(lo));
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
                asm("cfc2.ni %0, vi18" : "=r"(hi));
                if ((lo | hi) & 0xFFFFFF) {
                    func_001D4FB0(obj);
                    func_001D1F80(0, 1, 0);
                    func_001D4B20(obj);
                    func_001D4DA0();
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

    switch (stage) {
    case 0x0100:
    case 0x0101:
    case 0x0301:
    case 0x0400:
    case 0x0401:
    case 0x0600:
    case 0x0601:
    case 0x0700:
    case 0x0703:
    case 0x0800:
    case 0x0805:
    case 0x0801:
    case 0x0806:
    case 0x0803:
    case 0x0F00:
    case 0x1000:
    case 0x1001:
    case 0x1100:
    case 0x1500:
        func_001D5BD0();
        break;
    }
}
