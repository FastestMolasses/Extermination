// NEARMISS func_001A7870  (vram 0x001A7870, 0x30C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered: O(N^2) all-pairs entity check over D_00275B90[D_00275B98] (mark eligible bit-0.58-nonzero.type==-2.flagmatch entities in pass 1, then nested outer/inner scan comparing X-axis AABB overlap then Y-Z planar distance via func_0011E748(dy*dy+dz*dz), and on overlap eithe...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern float func_0011E748(float x);
extern int func_00128350(float x);
extern int func_001000C0(int a, double d);
extern unsigned char *D_00275B90;
extern short D_00275B98;

void func_001A7870(void)
{
    int *p;
    int i;
    int e;
    int a;

    if (D_00275B98 < 2) return;

    p = (int *)D_00275B90;
    for (i = 0; i < D_00275B98; i++) {
        e = *p;
        if ((*(unsigned char *)e & 1) &&
            (a = *(int *)(e + 0x58), a != 0) &&
            *(int *)a != 0 &&
            *(short *)(a + 0xA) == -2 &&
            (*(unsigned char *)(a + 6) & *(unsigned char *)(e + 0x5E))) {
            *(short *)(e + 0x50) = 1;
        } else {
            *(short *)(e + 0x50) = 0;
        }
        p++;
    }

    *(short *)0x70003B88 = D_00275B98;
    p = (int *)D_00275B90;
    *(short *)0x70003B88 = *(short *)0x70003B88 - 1;
    while (*(short *)0x70003B88 != 0) {
        int outer = *p;
        p++;
        if (*(unsigned short *)(outer + 0x50) != 0) {
            int *q = p;
            int oi;
            float o_px_r, o_py_r, o_pz_r, o_half, o_r;
            float o_px, o_py, o_pz;

            *(short *)0x70003B86 = *(short *)0x70003B88;

            oi = *(int *)(outer + 0x58);
            o_px_r = *(float *)(oi + 0x10);
            o_py_r = *(float *)(oi + 0xC);
            o_pz_r = *(float *)(oi + 0x14);
            o_half = *(float *)(oi + 0x1C);
            o_r = *(float *)(oi + 0x18);
            {
                int tbl = *(int *)(outer + (*(signed char *)(oi + 7) << 2) + 0x110);
                o_px = *(float *)(tbl + 0xC4) + o_px_r;
                o_py = *(float *)(tbl + 0xC0) + o_py_r;
                o_pz = *(float *)(tbl + 0xC8) + o_pz_r;
            }
            {
                float lo = o_px - o_half;
                float hi = o_px + o_half;

                while (*(short *)0x70003B86 != 0) {
                    int inner = *q;
                    *(short *)0x70003B86 = *(short *)0x70003B86 - 1;
                    q++;
                    if (*(unsigned short *)(inner + 0x50) != 0) {
                        int ii = *(int *)(inner + 0x58);
                        float i_px, i_py, i_pz;
                        float i_px_r, i_pz_r, i_half, i_py_r, i_r;
                        int tbl2;
                        i_px_r = *(float *)(ii + 0x10);
                        i_pz_r = *(float *)(ii + 0x14);
                        i_half = *(float *)(ii + 0x1C);
                        i_py_r = *(float *)(ii + 0xC);
                        i_r = *(float *)(ii + 0x18);
                        tbl2 = *(int *)(inner + (*(signed char *)(ii + 7) << 2) + 0x110);
                        i_px = *(float *)(tbl2 + 0xC4) + i_px_r;
                        i_pz = *(float *)(tbl2 + 0xC8) + i_pz_r;
                        {
                            float glo = i_px - i_half;
                            i_py = *(float *)(tbl2 + 0xC0) + i_py_r;
                            if (!(hi < glo) && (lo <= (i_px + i_half))) {
                                float dy = i_py - o_py;
                                float dz = i_pz - o_pz;
                                float len = func_0011E748(dy * dy + dz * dz);
                                float ov = (o_r + i_r) - len;
                                if (!(ov < 0.0f) && !(*(unsigned short *)(inner + 0x52) & 1)) {
                                    if (func_001000C0(func_00128350(len), 0.001) != 0) {
                                        *(float *)(inner + 0xB0) = *(float *)(inner + 0xB0) + ov;
                                    } else {
                                        *(float *)(inner + 0xB0) = *(float *)(inner + 0xB0) + ((dy * ov) / len);
                                        *(float *)(inner + 0xB8) = *(float *)(inner + 0xB8) + ((dz * ov) / len);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        *(short *)0x70003B88 = *(short *)0x70003B88 - 1;
    }
}
