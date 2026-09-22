// NEARMISS func_001D0720  (vram 0x001D0720, 0x548 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.28% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// FPR allocation, branch-delay scheduling and shared fallback flow differ; candidate is 24 bytes longer. Original-instruction oracle verifies 7680 state transitions and identical RNG calls under host float32 rules.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

/* Original face state at actor+0x90:
 * +0x40/+0x44 are two independently timed blend weights; +0x48..+0x58
 * approach five mouth targets at +0x94..+0xA4. Control state starts at +0x70.
 * The shared RNG determines waiting times and mouth shapes; geometry and
 * initial pool-slot contents are managed by the caller.
 *
 * Unknown second-state values follow the same talk gate as state 4.
 * The explicit shared label retains that original branch, which the older
 * decompilation incorrectly omitted. Mouth choice 0..4 and target 1..5 are
 * intentionally asymmetric in the original and must not be renumbered. */
extern int func_00122BB8(void);
extern int D_002513B0[4];
extern int D_002513C0[4];

void func_001D0720(char *actor) {
    char *face;
    char *control;
    int blink_state;
    int expression_state;
    int integer;
    int shape;
    int shape_index;
    char *weight_cursor;
    char *target_cursor;
    int count;
    float f4;
    float f0;
    float f1;
    float f2;

    face = *(char **)(actor + 0x90);
    control = face + 0x70;
    blink_state = *(int *)(face + 0x70);
    if (blink_state != 3) {
        switch (blink_state) {
        case 0:
            *(int *)(control + 4) = D_002513B0[(func_00122BB8() >> 16) & 3];
            *(int *)(control + 0) = 1;
            break;
        case 1:
            integer = *(int *)(control + 4) - 1;
            *(int *)(control + 4) = integer;
            if (integer <= 0) {
                *(int *)(control + 0) = 2;
            }
            break;
        case 2:
            f2 = *(float *)(face + 0x40);
            f0 = 1.0f - f2;
            f1 = f2 + (0.4f * f0);
            *(float *)(face + 0x40) = f1;
            if (!(f1 <= 0.95f)) {
                *(float *)(face + 0x40) = 1.0f;
                *(int *)(control + 0) = 3;
            }
            break;
        }
    } else {
        f2 = *(float *)(face + 0x40);
        f0 = -f2;
        f1 = f2 + (0.4f * f0);
        *(float *)(face + 0x40) = f1;
        if (f1 < 0.05f) {
            *(float *)(face + 0x40) = 0.0f;
            *(int *)(control + 0) = 0;
        }
    }

    expression_state = *(int *)(control + 8);
    if (expression_state != 4) {
        switch (expression_state) {
        case 0:
            if (*(unsigned char *)(control + 0x10) == 0) {
                integer = func_00122BB8() >> 16;
                integer *= 90;
                integer >>= 15;
                *(int *)(control + 0xC) = integer + 0x3C;
            } else {
                *(int *)(control + 0xC) = D_002513C0[(func_00122BB8() >> 16) & 3];
            }
            *(int *)(control + 8) = 1;
            break;
        case 1:
            integer = *(int *)(control + 0xC) - 1;
            *(int *)(control + 0xC) = integer;
            if (integer <= 0) {
                if (*(float *)(face + 0x44) < 0.5f) {
                    *(int *)(control + 8) = 2;
                } else {
                    *(int *)(control + 8) = 3;
                }
            }
            break;
        case 2:
            f2 = *(float *)(face + 0x44);
            f0 = 1.0f - f2;
            f1 = f2 + (0.1f * f0);
            *(float *)(face + 0x44) = f1;
            if (!(f1 <= 0.95f)) {
                *(float *)(face + 0x44) = 1.0f;
                *(int *)(control + 8) = 0;
            }
            break;
        case 3:
            f2 = *(float *)(face + 0x44);
            f0 = -f2;
            f1 = f2 + (0.1f * f0);
            *(float *)(face + 0x44) = f1;
            if (f1 < 0.05f) {
                *(float *)(face + 0x44) = 0.0f;
                *(int *)(control + 8) = 0;
            }
            break;
        default: goto expression_talk_gate;
        }
    } else {
expression_talk_gate:
        if (*(unsigned char *)(control + 0x10) != 0) {
            *(int *)(control + 8) = 0;
        }
    }

    weight_cursor = face + 4;
    if (*(unsigned char *)(control + 0x10) != 0) {
        integer = *(int *)(control + 0x14) - 1;
        *(int *)(control + 0x14) = integer;
        if (integer < 0) {
            shape = (func_00122BB8() >> 24) % 5;
            if (shape == *(int *)(control + 0x1C)) {
                shape = (*(int *)(control + 0x1C) + 1) % 5;
            }
            shape_index = 1;
            if (shape == *(int *)(control + 0x18)) {
                char *p;
                int i;
                i = 1;
                p = control + 4;
                do {
                    if (i == *(int *)(control + 0x1C)) {
                        *(float *)(p + 0x20) = 0.0f;
                    } else {
                        *(float *)(p + 0x20) = *(float *)(p + 0x20) * 0.1f;
                    }
                    i += 1;
                    p += 4;
                } while (i < 6);
            } else {
                target_cursor = control + 4;
                do {
                    f0 = (float)((func_00122BB8() >> 24) & 0x7F);
                    if (shape_index != shape) {
                        *(float *)(target_cursor + 0x20) = *(float *)(target_cursor + 0x20) * (0.1f + (f0 / 256.0f));
                    } else {
                        *(float *)(target_cursor + 0x20) = 0.8f + (f0 / 512.0f);
                    }
                    shape_index += 1;
                    target_cursor += 4;
                } while (shape_index < 6);
            }
            *(int *)(control + 0x1C) = *(int *)(control + 0x18);
            *(int *)(control + 0x18) = shape;
            integer = *(unsigned char *)(control + 0x11);
            if (integer != 2) {
                if (integer != 1) {
                    *(int *)(control + 0x14) = ((func_00122BB8() >> 16) % 5) + 3;
                } else {
                    *(int *)(control + 0x14) = ((func_00122BB8() >> 16) & 7) + 3;
                }
            } else {
                *(int *)(control + 0x14) = ((func_00122BB8() >> 16) & 0xF) + 3;
            }
        }
        weight_cursor = face + 4;
    }

    count = 1;
    target_cursor = control + 4;
    do {
        integer = *(unsigned char *)(control + 0x11);
        f4 = *(float *)(weight_cursor + 0x44);
        if (integer != 2) {
            if (integer != 1) {
                f4 += 0.4f * (*(float *)(target_cursor + 0x20) - f4);
            } else {
                f4 += 0.3f * (*(float *)(target_cursor + 0x20) - f4);
            }
        } else {
            f4 += 0.2f * (*(float *)(target_cursor + 0x20) - f4);
        }
        count += 1;
        *(float *)(weight_cursor + 0x44) = f4;
        weight_cursor += 4;
        target_cursor += 4;
    } while (count < 6);
}
