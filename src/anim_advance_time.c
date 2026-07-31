// NEARMISS anim_advance_time  (vram 0x001C64F0, 0x2F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Best build is mwcc991202 (93.16%) vs mwcc233 (92.39%). Body/control-flow fully recovered (per-frame anim clip-time advancer + event-table scan + rotation-keyframe re-resolve dispatch, matches docs/FINDINGS.md's anim_advance_time description). Residuals are pure compiler-scheduling artifacts, not logi...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern char *D_00275BF8;
extern void anim_clip_resolve(int base, short index);
extern int float_to_int(float t);
extern void func_001C8710(char *p, unsigned char c, float a);
extern void func_001C87C0(char *p, unsigned char c, float a);
extern void anim_sample_bones(char *p, unsigned char c, float a, float b);

short anim_advance_time(char *arg0, float dt) {
    float t;
    float step;
    short flags;
    char *hdr;
    short nrec;
    char *rec;
    int i;

    t = dt;
    flags = 0;
    while (!(t <= 0.0f)) {
        step = (t <= 1.0f) ? t : 1.0f;
        t -= step;

        anim_clip_resolve(*(int *)(arg0 + 0x40), *(short *)(arg0 + 0x2C));
        hdr = D_00275BF8;
        if (*(int *)(hdr + 0x14) != 0) {
            if (!(*(short *)(arg0 + 0x2C) & 0x8000)) {
                rec = hdr + *(int *)(hdr + 0x14);
                nrec = *(short *)rec;
                rec += 4;
                for (i = 0; i < nrec; i++, rec += 4) {
                    if (*(short *)rec == (short)float_to_int(*(float *)(arg0 + 0x3C))) {
                        flags |= *(short *)(rec + 2);
                        break;
                    }
                }
            }
        }

        if (*(float *)(arg0 + 0x3C) <= 1.0f) {
            if (*(short *)(arg0 + 0x2C) & 0x8000) {
                *(short *)(arg0 + 0x2C) &= 0x7FFF;
                *(float *)(arg0 + 0x3C) = (float)(unsigned int)*(unsigned short *)(D_00275BF8 + 2);
                *(float *)(arg0 + 0x3C) -= *(short *)(*(char **)(arg0 + 0x110) + 0x8E);
                func_001C8710(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), *(short *)(*(char **)(arg0 + 0x110) + 0x8E));
            } else if (*(short *)(hdr + 4) != -2) {
                if (*(short *)(hdr + 4) != -1) {
                    *(short *)(arg0 + 0x2C) = *(short *)(hdr + 4) | 0x8000;
                    flags |= 0x4000;
                    *(float *)(arg0 + 0x3C) = (float)*(short *)(D_00275BF8 + 6);
                    anim_clip_resolve(*(int *)(arg0 + 0x40), *(short *)(arg0 + 0x2C));
                    anim_sample_bones(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), 0.0f, *(float *)(arg0 + 0x3C));
                } else {
                    flags |= 0x3000;
                    *(float *)(arg0 + 0x3C) = (float)(unsigned int)*(unsigned short *)(hdr + 2);
                    func_001C8710(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), 0.0f);
                }
            } else {
                flags |= 0x1000;
            }
        } else {
            *(float *)(arg0 + 0x3C) -= step;
            func_001C87C0(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), step);
            if (*(short *)(arg0 + 0x2C) & 0x8000) {
                flags |= (short)0x8000;
            }
        }
    }

    return flags;
}
