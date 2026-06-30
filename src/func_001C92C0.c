// NEARMISS func_001C92C0  (vram 0x001C92C0, 0x1E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 86.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP even/odd register-coloring permutation (permuter-class, same as func_001509A0). Body, the keyframe-search loop, and the lerp/translation math are fully recovered (o233 86.84%); frame size (0x60), the s0-s3 callee-save set, and the gp_rel access of D_00275BEC (unlocked by -sdatathreshold 8) all...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void anim_decode_translation(char *src, float *dst);
extern void func_001C86A0(float *a, float *b, float *c, float t);
extern char *D_00275BEC;

void func_001C92C0(char *arg0, int arg1, int arg2) {
    char *e;
    unsigned short start, end;
    float factor, ang;

    e = D_00275BEC + *(int *)(D_00275BEC + arg1 * 4);
    *(unsigned short *)(arg0 + 0x6A) = 1;
    while (1) {
        start = *(unsigned short *)(e + 0xA);
        if (!(arg2 < (int)start)) {
            end = *(unsigned short *)(e + 0x16);
            if (arg2 < (int)end) {
                break;
            }
        }
        e += 0xC;
        *(unsigned short *)(arg0 + 0x6A) = *(unsigned short *)(arg0 + 0x6A) + 1;
    }
    factor = (float)arg2 - (float)(unsigned int)start;
    ang = (float)(unsigned int)start;
    anim_decode_translation(e, (float *)(arg0 + 0x18));
    ang = (float)(unsigned int)*(unsigned short *)(e + 0x16) - ang;
    anim_decode_translation(e + 0xC, (float *)(arg0 + 0x24));
    func_001C86A0((float *)(arg0 + 0x24), (float *)(arg0 + 0x24), (float *)(arg0 + 0x18), ang);
    *(float *)(arg0 + 0x5C) = (float)(unsigned int)end - (float)arg2;
    *(float *)(arg0 + 0x18) = *(float *)(arg0 + 0x18) + *(float *)(arg0 + 0x24) * factor;
    *(float *)(arg0 + 0x1C) = *(float *)(arg0 + 0x1C) + *(float *)(arg0 + 0x28) * factor;
    *(float *)(arg0 + 0x20) = *(float *)(arg0 + 0x20) + *(float *)(arg0 + 0x2C) * factor;
}
