// NEARMISS func_0013BBB0  (vram 0x0013BBB0, 0x2B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word
// body, matching wave 2026-10-28). Body-correct: same calls, stores and conditions as the original.
// Remaining differences: the switch on the current clip keeps its value in a2 in the original
// and in a0 here (switch-on-local short/int, 0x7FFF mask and local declaration orders tried);
// everything else matches. anim_clip_init is declared with a `short clip` parameter here: the
// original passes the short local straight through without re-extending it, which mwcc only
// does for a short parameter (the definition in src/anim_clip_init.c says int; it only stores
// the value to a short field, so a short parameter there is plausible).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Picks the turn clip for an actor from two angles in its state block st:
// a = st+0x5C, b = st+0x50. Clip 1 when a < b; otherwise clip 4 when
// b >= PI/4; otherwise, with d = a - b: when a > 0, clip 0 if d == 0 or
// d < PI/4 and clip 4 if d >= PI/4; when a <= 0 it stays clip 1.
// If the actor's current clip (e+0x2C without bit 0x8000) differs, it
// computes t = func_001C6160(e) - e+0x3C and splits it with modff, then by
// the CURRENT clip: from clip 0 -> clip 1 only when the integer part of t is
// 44, else to the new clip at frame min((1 + t) / 2, 23); from clip 4 ->
// clip 1 only when that integer part is 22, else frame min(2 (1 + t), 45);
// from clip 1 or any other clip: anim_clip_init(e, clip, 5.0f, 0.0f).
// Note: the integer part used is modff's iptr (written to the stack), not t.
extern int func_001C6160(char *e);
extern float func_0011E0A8(float x, float *iptr);
extern int float_to_int(float t);
extern void anim_clip_init(char *e, short clip, float blend, float frame);

void func_0013BBB0(char *e, char *st) {
    short anim;
    float a;
    float b;
    float d;
    float t;
    float frame;
    float ip;

    anim = 1;
    a = *(float *)(st + 0x5C);
    b = *(float *)(st + 0x50);
    if (!(a < b)) {
        if (!(b < 0.7853982f)) {
            anim = 4;
        } else {
            d = a - b;
            if (d == 0.0f) {
                if (!(a <= 0.0f)) {
                    anim = 0;
                }
            } else if (!(a <= 0.0f)) {
                if (!(d < 0.7853982f)) {
                    anim = 4;
                } else {
                    anim = 0;
                }
            }
        }
    }
    if ((short)(*(short *)(e + 0x2C) & ~0x8000) == anim) {
        return;
    }
    t = (float)func_001C6160(e) - *(float *)(e + 0x3C);
    func_0011E0A8(t, &ip);
    switch ((short)(*(short *)(e + 0x2C) & ~0x8000)) {
    case 0:
        if (anim == 1) {
            if (float_to_int(ip) == 44) {
                anim_clip_init(e, anim, 0.0f, 0.0f);
                return;
            }
        } else {
            frame = (1.0f + t) / 2.0f;
            if (!(frame <= 23.0f)) {
                frame = 23.0f;
            }
            anim_clip_init(e, anim, 0.0f, frame);
        }
        break;
    case 4:
        if (anim == 1) {
            if (float_to_int(ip) == 22) {
                anim_clip_init(e, anim, 0.0f, 0.0f);
            }
        } else {
            frame = 2.0f * (1.0f + t);
            if (!(frame <= 45.0f)) {
                frame = 45.0f;
            }
            anim_clip_init(e, anim, 0.0f, frame);
        }
        break;
    case 1:
    default:
        anim_clip_init(e, anim, 5.0f, 0.0f);
        break;
    }
}
