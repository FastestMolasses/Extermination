// NEARMISS func_0017ABA0  (vram 0x0017ABA0, 0x3CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Residual is confined to the st2==0x31/0x34 pitch-band-scale sub-block: (1) a genuine mwcc branch-lowering/scheduling choice for the `pitch<=0.3f || !(pitch<0.7f)` OR (bc1t/bc1f block-layout swap, tried both operand orderings of the OR, neither reproduced target's exact layout), and (2) target com...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011E2A8(float);
extern float func_001B1470(float);
extern int func_001B5DC0(unsigned char);

extern unsigned char D_00810E64;
extern unsigned char D_00810E65;

void func_0017ABA0(char *arg0) {
    float rate[4];
    float f20;
    int idx;
    float step;
    float pitch;
    unsigned char stance;

    stance = *(unsigned char *)(arg0 + 0x1F0);
    if (stance == 0x31 || stance == 0x34) {
        rate[0] = 0.0f;
        rate[1] = 0.0025f;
        rate[2] = 0.005f;
        rate[3] = 0.015f;
        f20 = 1.0f;
    } else {
        rate[0] = 0.0f;
        rate[1] = 0.0016666666f;
        rate[2] = 0.005f;
        rate[3] = 0.01f;
        f20 = 1.5f;
    }

    idx = func_001B5DC0(D_00810E64);
    if (idx != 0) {
        float ang;
        float newyaw;

        *(char *)(arg0 + 0x302) = 1;
        pitch = *(float *)(arg0 + 0x278);
        if (pitch == 0.5f) {
            ang = 1.0f;
        } else if (!(pitch <= 0.5f)) {
            *(float *)0x70003A20 = 0.5f + (0.6f * (pitch - 0.5f));
            ang = func_0011E2A8(3.1415927f * *(float *)0x70003A20);
        } else {
            *(float *)0x70003A20 = 0.5f - (0.6f * (0.5f - pitch));
            ang = func_0011E2A8(3.1415927f * *(float *)0x70003A20);
        }
        step = rate[idx] / ang;
        if (*(unsigned char *)(arg0 + 0x275) == 4) {
            step *= 1.5f;
        }
        if (D_00810E64 >= 0x80) {
            newyaw = *(float *)(arg0 + 0x27C) + step;
            *(float *)(arg0 + 0x27C) = newyaw;
            if (newyaw > 1.0f) {
                *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) - ((newyaw - 1.0f) * f20));
                *(float *)(arg0 + 0x27C) = 1.0f;
            }
        } else {
            newyaw = *(float *)(arg0 + 0x27C) - step;
            *(float *)(arg0 + 0x27C) = newyaw;
            if (newyaw < 0.0f) {
                *(float *)(arg0 + 0xC4) = func_001B1470(*(float *)(arg0 + 0xC4) + (-newyaw * f20));
                *(float *)(arg0 + 0x27C) = 0.0f;
            }
        }
    }

    idx = func_001B5DC0(D_00810E65);
    if (idx != 0) {
        float lim;
        float newpitch;
        unsigned char st2;
        unsigned int band;

        *(char *)(arg0 + 0x302) = 1;
        step = rate[idx];
        st2 = *(unsigned char *)(arg0 + 0x1F0);
        if (st2 == 0x31 || st2 == 0x34) {
            pitch = *(float *)(arg0 + 0x278);
            if (pitch <= 0.3f || !(pitch < 0.7f)) {
                step *= 1.5f;
            }
        }
        band = st2 - 0x31;
        lim = (band < 2U) ? 1.0f : 0.75f;
        if (*(unsigned char *)(arg0 + 0x275) == 4) {
            step *= 1.8f;
        }
        if (D_00810E65 >= 0x80) {
            newpitch = *(float *)(arg0 + 0x278) + (step / 2.0f);
            *(float *)(arg0 + 0x278) = newpitch;
            if (newpitch > lim) {
                *(float *)(arg0 + 0x278) = lim;
            }
        } else {
            newpitch = *(float *)(arg0 + 0x278) - (step / 2.0f);
            *(float *)(arg0 + 0x278) = newpitch;
            if (newpitch < 0.0f) {
                *(float *)(arg0 + 0x278) = 0.0f;
            }
        }
    }
}
