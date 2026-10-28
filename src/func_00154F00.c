// NEARMISS func_00154F00  (vram 0x00154F00, 0x2A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.60% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word
// body, matching wave 2026-10-28). Keys found: the spark records are arrays inside the sub-block
// indexed by i (the original walks two pointers based at the sub-block itself), the random
// increments are spelled vy = vy + (...), and the "fast" flag is one ternary.
// Remaining differences: the "fast" flag setup (the original zeroes it in both compare delay
// slots and keeps two unreachable zeroing copies; ours fills one slot with a nop), the
// temporary register of the first random-velocity sum (v1 vs v0) and of the 14x one, and two
// float temporaries (f1/f0 for the 0.5 halving, f3/f1 for the height scale). Tried: if/else,
// nested-if and ternary forms of the flag, local order, mwcc24 (same) and mwcc 991202 (90.0%),
// operand order of the float products, a rand local, short casts.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Spark-column effect draw: saves the actor position quad (self+0xB0), then for
// each of 12 spark records in the sub-block self+0x1F0 that is active it moves
// the height by its velocity, applies gravity (8 per tick when the linked object
// self+0x20 has its +0x80 value above 0.5, else 1), bounces at height 100 with a
// random upward speed (3..7) and below 128 adds a random kick (28..41 when fast,
// 3..7 otherwise), halves velocities >= 8 in slow mode, then sets the fade
// (self+0x8C = sub+0xC scaled by life/16 while life < 16), the scale
// (self+0x60/+0x68 = scale/256, self+0x64 = height*life/65536) and the x/z
// position from the record's pair, refreshes the transform (func_001C6380) and
// draws through the hook at self+0x4C. Finally restores the saved position and
// refreshes the transform again.
extern int float_to_int(float arg);
extern void func_00102948(void *dst, void *src);
extern int func_00122BB8(void);
extern void func_001C6380(unsigned char *self);

typedef struct Spark {
    short y;
    short vy;
    short life;
    short scale;
    short active;
} Spark;

typedef struct SparkXZ {
    float x;
    float z;
} SparkXZ;

typedef struct SparkState {
    char pad00[0xC];
    float fade;
    char pad10[0xC];
    SparkXZ xz[12];
    Spark sp[12];
} SparkState;

void func_00154F00(char *self) {
    SparkState *st = (SparkState *)(self + 0x1F0);
    char *link;
    int i;
    int fast;
    float h;
    float s;
    int saved[4];

    func_00102948(saved, self + 0xB0);
    link = *(char **)(self + 0x20);
    fast = (link == 0 || *(float *)(link + 0x80) <= 0.5f) ? 0 : 1;
    for (i = 0; i < 12; i++) {
        if (st->sp[i].active != 0) {
            st->sp[i].y += st->sp[i].vy;
            if (fast) {
                st->sp[i].vy -= 8;
            } else {
                st->sp[i].vy -= 1;
            }
            if (st->sp[i].y < 100) {
                st->sp[i].y = 100;
                st->sp[i].vy = (((func_00122BB8() >> 16) * 5) >> 15) + 3;
            } else if (st->sp[i].y < 128) {
                if (fast) {
                    st->sp[i].vy = st->sp[i].vy + ((((func_00122BB8() >> 16) * 14) >> 15) + 28);
                } else {
                    st->sp[i].vy = st->sp[i].vy + ((((func_00122BB8() >> 16) * 5) >> 15) + 3);
                }
            }
            if (!fast && st->sp[i].vy >= 8) {
                st->sp[i].vy = float_to_int(0.5f * (float)st->sp[i].vy);
            }
            h = 1.5258789e-05f * (float)(st->sp[i].y * st->sp[i].life);
            if (st->sp[i].life < 16) {
                *(float *)(self + 0x8C) = ((float)st->sp[i].life / 16.0f) * st->fade;
            } else {
                *(float *)(self + 0x8C) = st->fade;
            }
            s = 0.00390625f * (float)st->sp[i].scale;
            *(float *)(self + 0x68) = s;
            *(float *)(self + 0x60) = s;
            *(float *)(self + 0x64) = h;
            *(float *)(self + 0xB0) = st->xz[i].x;
            *(float *)(self + 0xB8) = st->xz[i].z;
            func_00102948(self + 0xB0, self + 0xB0);
            func_001C6380((unsigned char *)self);
            (*(void (**)(char *))(self + 0x4C))(self);
        }
    }
    func_00102948(self + 0xB0, saved);
    func_001C6380((unsigned char *)self);
}
