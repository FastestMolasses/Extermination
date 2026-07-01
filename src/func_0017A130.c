// NEARMISS func_0017A130  (vram 0x0017A130, 0x6C8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.94% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-order wall (highest-vs-lowest saved reg for the sole long-lived param); confirmed via isolated probes, not source-fixable.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// anim_matrix_dispatch: per-frame bone-matrix routing for a multi-clip actor.
// Blends between up to 3 world/local matrix buffers (D_00286340, D_00287140,
// D_00287F40/D_00288D40) based on two blend fractions at obj+0x278 and
// obj+0x27C, then publishes the fully blended matrices to the per-bone
// scratch table (D_00275B40) and applies a post-process (func_00102760) to
// each of the first 3 words per bone matrix, before handing off to the clip
// arbiter func_001749F0.
typedef struct Mat4 { unsigned char _b[0x40]; } Mat4;
extern int *D_00275B40;
extern Mat4 D_00286340;
extern Mat4 D_00287140;
extern Mat4 D_00287F40;
extern Mat4 D_00288D40;
extern short func_0017A0B0(unsigned char *, int);
extern void func_00179BC0(unsigned char *, int, Mat4 *);
extern void func_00179CA0(Mat4 *, Mat4 *, Mat4 *, float);
extern void func_00102760(void *, void *);
extern int func_001749F0(unsigned char *, int, int, float);

void func_0017A130(unsigned char *obj)
{
    int i;
    Mat4 *m1, *m2, *m3;

    if (*(float *)(obj + 0x278) <= 0.0f) {
        func_00179BC0(obj, func_0017A0B0(obj, 2), &D_00287140);
    } else if (*(float *)(obj + 0x278) >= 1.0f) {
        func_00179BC0(obj, func_0017A0B0(obj, 1), &D_00287140);
    } else {
        func_00179BC0(obj, func_0017A0B0(0, 0), &D_00288D40);
        if (*(float *)(obj + 0x278) <= 0.5f) {
            *(float *)0x70003A20 = 0.5f - *(float *)(obj + 0x278);
            func_00179BC0(obj, func_0017A0B0(obj, 2), &D_00287F40);
        } else {
            *(float *)0x70003A20 = *(float *)(obj + 0x278) - 0.5f;
            func_00179BC0(obj, func_0017A0B0(obj, 1), &D_00287F40);
        }
        m1 = &D_00287F40;
        m2 = &D_00288D40;
        m3 = &D_00287140;
        for (i = 0; i < obj[0xC]; i++) {
            func_00179CA0(m3, m2, m1, 2.0f * *(float *)0x70003A20);
            m1++;
            m2++;
            m3++;
        }
    }

    if (*(float *)(obj + 0x27C) <= 0.5f) {
        if (*(float *)(obj + 0x278) <= 0.0f) {
            func_00179BC0(obj, func_0017A0B0(obj, 8), &D_00286340);
        } else if (*(float *)(obj + 0x278) >= 1.0f) {
            func_00179BC0(obj, func_0017A0B0(obj, 7), &D_00286340);
        } else {
            func_00179BC0(obj, func_0017A0B0(obj, 4), &D_00288D40);
            if (*(float *)(obj + 0x278) <= 0.5f) {
                *(float *)0x70003A20 = 0.5f - *(float *)(obj + 0x278);
                func_00179BC0(obj, func_0017A0B0(obj, 8), &D_00287F40);
            } else {
                *(float *)0x70003A20 = *(float *)(obj + 0x278) - 0.5f;
                func_00179BC0(obj, func_0017A0B0(obj, 7), &D_00287F40);
            }
            m1 = &D_00287F40;
            m2 = &D_00288D40;
            m3 = &D_00286340;
            for (i = 0; i < obj[0xC]; i++) {
                func_00179CA0(m3, m2, m1, 2.0f * *(float *)0x70003A20);
                m1++;
                m2++;
                m3++;
            }
        }
    } else {
        if (*(float *)(obj + 0x278) <= 0.0f) {
            func_00179BC0(obj, func_0017A0B0(obj, 6), &D_00286340);
        } else if (*(float *)(obj + 0x278) >= 1.0f) {
            func_00179BC0(obj, func_0017A0B0(obj, 5), &D_00286340);
        } else {
            func_00179BC0(obj, func_0017A0B0(obj, 3), &D_00288D40);
            if (*(float *)(obj + 0x278) <= 0.5f) {
                *(float *)0x70003A20 = 0.5f - *(float *)(obj + 0x278);
                func_00179BC0(obj, func_0017A0B0(obj, 6), &D_00287F40);
            } else {
                *(float *)0x70003A20 = *(float *)(obj + 0x278) - 0.5f;
                func_00179BC0(obj, func_0017A0B0(obj, 5), &D_00287F40);
            }
            m1 = &D_00287F40;
            m2 = &D_00288D40;
            m3 = &D_00286340;
            for (i = 0; i < obj[0xC]; i++) {
                func_00179CA0(m3, m2, m1, 2.0f * *(float *)0x70003A20);
                m1++;
                m2++;
                m3++;
            }
        }
    }

    if (*(float *)(obj + 0x27C) <= 0.0f) {
        *(float *)0x70003A20 = 0.5f;
    } else if (*(float *)(obj + 0x27C) >= 1.0f) {
        *(float *)0x70003A20 = 0.5f;
    } else if (*(float *)(obj + 0x27C) <= 0.5f) {
        *(float *)0x70003A20 = 0.5f - *(float *)(obj + 0x27C);
    } else {
        *(float *)0x70003A20 = *(float *)(obj + 0x27C) - 0.5f;
    }

    m1 = &D_00286340;
    m2 = &D_00287140;
    for (i = 0; i < obj[0xC]; i++) {
        func_00179CA0((Mat4 *)(*(int *)((char *)D_00275B40 + i * 4) + 0x90), m2, m1, 2.0f * *(float *)0x70003A20);
        m1++;
        m2++;
    }

    for (i = 0; i < obj[0xC]; i++) {
        int j;
        int *base = (int *)((char *)D_00275B40 + i * 4);
        unsigned char *node = *(unsigned char **)base + 0x90;
        for (j = 0; j < 3; j++) {
            func_00102760(node, node);
            node += 0x10;
        }
    }

    obj[0x303] = 1;
    func_001749F0(obj, func_0017A0B0(obj, 0), 0, (float)*(short *)(obj + 0x276));
}
