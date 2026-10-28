// NEARMISS anim_advance_time  (vram 0x001C64F0, 0x2F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.76% via mwcc 2.3-991202 (-O4,p -sdatathreshold 4); same C scores 94.28% under mwcc 2.3.3
// (95.02% with the clamp spelled `if (!(dt <= 1.0f)) step = 1.0f; else step = dt;`).
// Per-frame anim clip-time advancer: steps the clip time in slices of at most 1.0, scans the
// clip's event table for the current frame, and handles clip end (loop / hold / chain to next clip).
// Fixes this wave: callees take int counts (not unsigned char), anim_clip_resolve takes a pointer
// base (twin anim_clip_init), the function returns int (the original sign-extends flags on return),
// dt itself is the loop variable, the frame time is read once into `cur`, ternary clamp.
// Remaining differences: compiler mix only. Under 991202: the zero constant's setup position, the
// event-loop test (at vs v0 and 2.3.3-style nop padding), one unfilled beq slot and a trailing nop.
// Under 2.3.3: the original deletes the instruction copied into a filled delay slot (bc1tl clamp,
// bnel loop step, 0x1000 / 0x7FFF arms), which only 991202 does. Needs the 2.3.1.01 compiler.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 4

extern char *D_00275BF8;
extern void anim_clip_resolve(void *base, short index);
extern int float_to_int(float t);
extern void func_001C8710(char *p, int c, float a);
extern void func_001C87C0(char *p, int c, float a);
extern void anim_sample_bones(char *p, int c, float a, float b);

int anim_advance_time(char *arg0, float dt) {
    float step;
    float cur;
    int i;
    short flags;
    char *rec;
    short nrec;
    char *hdr;

    flags = 0;
    while (!(dt <= 0.0f)) {
        step = (dt > 1.0f) ? 1.0f : dt;
        dt -= step;

        anim_clip_resolve(*(void **)(arg0 + 0x40), *(short *)(arg0 + 0x2C));
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
        cur = *(float *)(arg0 + 0x3C);
        if (cur <= 1.0f) {
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
                    anim_clip_resolve(*(void **)(arg0 + 0x40), *(short *)(arg0 + 0x2C));
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
            *(float *)(arg0 + 0x3C) = cur - step;
            func_001C87C0(arg0 + 0x110, *(unsigned char *)(arg0 + 0xC), step);
            if (*(short *)(arg0 + 0x2C) & 0x8000) {
                flags |= (short)0x8000;
            }
        }
    }

    return flags;
}
