// NEARMISS func_001634A0  (vram 0x001634A0, 0x53C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual classes: (1) FP-register-coloring in the velocity-integration expr (self+0x2E4 = self+0x2E4 + 2.0f*self+0x2EC) -- target uses fv0f/fv1f pairing, mwcc uses ft0/ft1; unaffected by any source reordering tried. (2) A guard-combination polarity flip: if (func_002243F0(arg0)==0 && func_001...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Animation-clip state machine (self+6 state byte), sibling of func_001612D0/func_0016A8B0.
// state 0: advance to state 1, pick start clip (0x69 blended from func_001C61D0, or 0x6A)
//   via func_001749A0/anim_clip_arbiter, seed self+0x2F4 from self+0xB4.
// state 1: on self+0x200 bit 0x1000, advance to state 2, pick a target speed (self+0x38) by
//   self+0x25C threshold, blend clips 0x6B/0x6C, seed per-index float tables D_002485D0/
//   D_002485B0 (index self+0x25C) into self+0x2EC/0x38/0x2E4/0x270, reset self+0x2E/0x25F.
// state 2: velocity integration into self+0xB4/0x2E4, transitions to state 3 when
//   self+0xB4<=self+0x2F4 or self+0x314==1, clamps/animates via func_00175900/func_0017C580,
//   exits to func_0021D250 when self+0x23A==0x5D.
// state 3: decays self+0x38 by self+0x2E0, applies func_00179880 velocity blend, similar
//   integration + exit as state 2 but on idle resets to a fresh clip 0x72 (self+5/6/0x1F0).
// state 0x63: terminal -- just calls func_0021D2E0(0x78, 0).
extern void anim_clip_arbiter(char *p, int clip, float speed, float f);
extern float func_0011DE90(int idx, float speed);
extern float func_0011E2A8(float speed);
extern void func_001749A0(char *e, int clip, int flags, float blend);
extern void func_001751A0(char *p);
extern void func_00175900(char *p, int f);
extern int func_001760C0(char *p, char *dst, int flags, float speed);
extern void func_00178B90(char *p, unsigned char a);
extern void func_00178EC0(char *p);
extern void func_00179880(char *p, char *out);
extern void func_0017C580(char *p);
extern int func_0017C860(char *p, float v);
extern void func_0017DEB0(char *p);
extern int func_001C61D0(int idx, int clip, unsigned char st);
extern void func_0021D250(char *p, int a);
extern void func_0021D2E0(char *p, int a, int b);
extern int func_002243F0(char *p);

extern float D_002485B0[];
extern float D_002485D0[];

void func_001634A0(char *arg0) {
    unsigned char st = *(unsigned char *)(arg0 + 6);

    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 7) = 0;
        if (*(unsigned char *)(arg0 + 0x25C) < 2) {
            func_001749A0(arg0, 0x6A, 0, 8.0f);
        } else {
            float speed = (float)func_001C61D0(*(int *)(arg0 + 0x40), 0x69, st);
            float k = 4.0f;
            *(volatile float *)0x70003A20 = speed;
            anim_clip_arbiter(arg0, 0x69, k, speed - k);
        }
        *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
        return;

    case 1:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            unsigned char idx;
            float *tbl;

            *(unsigned char *)(arg0 + 6) = st + 1;
            if (*(unsigned char *)(arg0 + 0x25C) < 2) {
                *(float *)(arg0 + 0x38) = 4.1f;
                func_00178B90(arg0, 1);
                func_001749A0(arg0, 0x6C, 0, 0.0f);
            } else {
                *(float *)(arg0 + 0x38) = 6.3f;
                func_00178B90(arg0, 1);
                func_001749A0(arg0, 0x6B, 0, 0.0f);
                *(float *)(arg0 + 0x2EC) = -0.03f;
            }
            idx = *(unsigned char *)(arg0 + 0x25C);
            *(float *)(arg0 + 0x2EC) = D_002485D0[idx];
            idx = *(unsigned char *)(arg0 + 0x25C);
            tbl = &D_002485B0[idx * 2];
            *(float *)(arg0 + 0x38) = tbl[0] * func_0011DE90(idx, tbl[1]);
            *(float *)(arg0 + 0x2E4) = tbl[0] * func_0011E2A8(tbl[1]);
            *(float *)(arg0 + 0x270) = *(float *)(arg0 + 0x38) / 3.0f;
            *(short *)(arg0 + 0x2E) = 0;
            *(unsigned char *)(arg0 + 0x25F) = 1;
            func_0017DEB0(arg0);
            return;
        }
        return;

    case 2: {
        int r = func_002243F0(arg0);
        if (r == 0 && func_0017C860(arg0, *(float *)(arg0 + 0x2E4)) == 0) {
            return;
        }
        {
            func_001751A0(arg0);
            if (*(int *)(arg0 + 0x24C) == 1) {
                if (*(unsigned char *)(arg0 + 0x23F) >= 2) {
                    float v = *(float *)(arg0 + 0x38) * 0.9f;
                    *(float *)(arg0 + 0x38) = v;
                    if (v < *(float *)(arg0 + 0x270)) {
                        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x270);
                    }
                }
            } else {
                func_00178EC0(arg0);
            }
            func_00178B90(arg0, 1);
            {
                float k = 2.0f * *(float *)(arg0 + 0x2EC);
                float v = *(float *)(arg0 + 0x2E4) + k;
                *(float *)(arg0 + 0x2E4) = v;
                *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + v;
            }
            if (!(*(float *)(arg0 + 0x2E4) <= 0.0f)) {
                if (func_001760C0(arg0, arg0 + 0xB0, 0, 18.0f) != 0) {
                    *(float *)(arg0 + 0x2E4) = 0.0f;
                }
            } else {
                *(unsigned char *)(arg0 + 0x25F) = 2;
            }
            if (*(float *)(arg0 + 0xB4) <= *(float *)(arg0 + 0x2F4) || *(unsigned char *)(arg0 + 0x314) == 1) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(float *)(arg0 + 0x2E0) = *(float *)(arg0 + 0x38) / 30.0f;
                *(float *)(arg0 + 0x2EC) = *(float *)(arg0 + 0x2E4);
            }
            if (*(float *)(arg0 + 0x2E4) <= 0.0f) {
                func_00175900(arg0, 1);
                if (*(unsigned char *)(arg0 + 0xA) != 0) {
                    *(float *)(arg0 + 0x38) = 0.0f;
                    if (r == 0) {
                        func_0017C580(arg0);
                    }
                }
            }
            if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
                func_0021D250(arg0, 0);
                return;
            }
        }
        break;
    }

    case 3: {
        int r = func_002243F0(arg0);
        if (r == 0 && func_0017C860(arg0, *(float *)(arg0 + 0x2E4)) == 0) {
            return;
        }
        {
            func_001751A0(arg0);
            if (*(float *)(arg0 + 0x38) > *(float *)(arg0 + 0x2E0)) {
                *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - *(float *)(arg0 + 0x2E0);
                func_00178EC0(arg0);
            } else {
                *(float *)(arg0 + 0x38) = 0.0f;
            }
            func_00178B90(arg0, 1);
            func_00179880(arg0, arg0 + 0x2E4);
            if (!(*(float *)(arg0 + 0x2E4) <= 0.0f)) {
                if (func_001760C0(arg0, arg0 + 0xB0, 0, 18.0f) != 0) {
                    *(float *)(arg0 + 0x2E4) = 0.0f;
                }
            } else {
                *(unsigned char *)(arg0 + 0x25F) = 2;
            }
            func_00175900(arg0, 1);
            if (*(unsigned char *)(arg0 + 0xA) != 0) {
                *(float *)(arg0 + 0x38) = 0.0f;
                if (r == 0) {
                    func_0017C580(arg0);
                }
            } else if (*(float *)(arg0 + 0x38) <= 0.0f && r == 0) {
                *(char *)(arg0 + 5) = 7;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg0 + 0x1F0) = 0xD;
                func_001749A0(arg0, 0x72, 0, 8.0f);
                *(float *)(arg0 + 0x2EC) = *(float *)(arg0 + 0x2E4);
            }
            if (*(unsigned char *)(arg0 + 0x23A) == 0x5D) {
                func_0021D250(arg0, 0);
                return;
            }
        }
        break;
    }

    case 0x63:
        func_0021D2E0(arg0, 0x78, 0);
        break;
    }
}
