// NEARMISS func_001E4A00  (vram 0x001E4A00, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 38.04%; matching wave 2026-10-28).
// Remaining differences: float saved-register colouring only (the original keeps t in f20,
// size in f22, w in f21, the frame value in f23; this C gets t f22 / size f21 / w f20) and
// the address setup order inside the two VU0 lerp blocks (the original materialises the
// scratch, ramp and key pointers after the divide, in a3 / a2 / v0). Declaration-order,
// statement-order, param-reuse and inline-helper variants tried (about 20 trials).
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Fixes found: the colour ramp lerp is VU0 code (b + (a - b) * u, clamped to 0..255 with the
// key quadword {u, 255}); the "t > 0.2" arm is the VU0 path and the copy arm is the else;
// w is computed before the frame index.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Impact-flash sprite: while age < 1.8, t = age / 1.8 picks two colours from the ramps
// D_002541F0 and D_00254210 (held at the first stop until t = 0.2, then lerped with
// u = (t - 0.2) / 0.8 and clamped to 0..255 in the scratchpad quad 0x700038B0), packs each to
// RGBA through func_00128250 and draws two billboards at obj + 0x30 with func_001CD520: the
// animated frame D_00253C50[10 t] at size w = size (1 + 2t), then the halo sprite at 2w.

typedef struct { float x, y, z, w; } Vec4;

extern int  float_to_int(float x);
extern int  func_00128250(float x);
extern void func_00102948(void *dst, const void *src);

extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);

extern Vec4 D_002541F0[2];
extern Vec4 D_00254210[2];
extern unsigned long long D_00253C50[10];
extern Vec4 D_700038B0;

void func_001E4A00(char *obj, float age, float size)
{
    unsigned int rgba1, rgba2;
    float t, frame_f, w, u;
    Vec4 *dst;
    Vec4 *ramp;
    Vec4 *pk;
    Vec4 k1;
    Vec4 k2;

    if (!(age < 1.8f)) {
        return;
    }

    t       = age / 1.8f;
    w       = size + (2.0f * size) * t;
    frame_f = 10.0f * t;

    if (t > 0.2f) {
        k1.x = (t - 0.2f) / 0.8f;
        k1.y = 255.0f;
        dst = &D_700038B0;
        ramp = D_002541F0;
        pk = &k1;
        asm("lqc2 vf28, 0x0(%0)" : : "r"(ramp));
        asm("lqc2 vf29, 0x10(%0)" : : "r"(ramp));
        asm("lqc2 vf30, 0x0(%0)" : : "r"(pk));
        asm("vsub.xyzw vf1, vf29, vf28");
        asm("vmulx.xyzw vf1, vf1, vf30x");
        asm("vadd.xyzw vf1, vf1, vf28");
        asm("vmaxx.xyzw vf1, vf1, vf0x");
        asm("vminiy.xyzw vf1, vf1, vf30y");
        asm("sqc2 vf1, 0x0(%0)" : : "r"(dst));
    } else {
        func_00102948(&D_700038B0, D_002541F0);
    }
    rgba1 = func_00128250(D_700038B0.x);
    rgba1 |= func_00128250(D_700038B0.y) << 8;
    rgba1 |= func_00128250(D_700038B0.z) << 16;
    rgba1 |= func_00128250(D_700038B0.w) << 24;

    if (t > 0.2f) {
        k2.x = (t - 0.2f) / 0.8f;
        k2.y = 255.0f;
        dst = &D_700038B0;
        ramp = D_00254210;
        pk = &k2;
        asm("lqc2 vf28, 0x0(%0)" : : "r"(ramp));
        asm("lqc2 vf29, 0x10(%0)" : : "r"(ramp));
        asm("lqc2 vf30, 0x0(%0)" : : "r"(pk));
        asm("vsub.xyzw vf1, vf29, vf28");
        asm("vmulx.xyzw vf1, vf1, vf30x");
        asm("vadd.xyzw vf1, vf1, vf28");
        asm("vmaxx.xyzw vf1, vf1, vf0x");
        asm("vminiy.xyzw vf1, vf1, vf30y");
        asm("sqc2 vf1, 0x0(%0)" : : "r"(dst));
    } else {
        func_00102948(&D_700038B0, D_00254210);
    }
    rgba2 = func_00128250(D_700038B0.x);
    rgba2 |= func_00128250(D_700038B0.y) << 8;
    rgba2 |= func_00128250(D_700038B0.z) << 16;
    rgba2 |= func_00128250(D_700038B0.w) << 24;

    func_001CD520(0, 2, (Vec4 *)(obj + 0x30), D_00253C50[float_to_int(frame_f)], rgba1, w, w, size / 2.0f);
    func_001CD520(0, 2, (Vec4 *)(obj + 0x30), 0x20045BA5154222DCULL, rgba2, 2.0f * w, 2.0f * w, size / 2.0f);
}
