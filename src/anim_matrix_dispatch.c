// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.55% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad float at 0x70003A20 (the blend fraction shared by the
//    blend loops) is a relocated extern, D_70003A20 (idiom-32), not a
//    literal address: with the literal, mwcc treated the upper half as a
//    constant and speculated it into the three blend loops' back-edge delay
//    slots, which the original leaves empty. Under -sdatathreshold 4 it is
//    over-declared as an array (idiom #20) so it does not go gp-relative.
//  - Earlier (decomp2 lane round 4): the mid-blend branch publishes
//    anim_slot_index(obj, 0); anim_clip_arbiter's blend and frame are
//    floats and its slot id is short; obj is a local copy declared first
//    (kept in $s0); the last two loops use their own pointers, counters and
//    byte offsets; D_00275B40 is gp-relative.
//
// anim_matrix_dispatch: per-frame bone-matrix routing for a multi-clip actor.
// Blends between up to 3 world/local matrix buffers (D_00286340, D_00287140,
// D_00287F40/D_00288D40) based on two blend fractions at obj+0x278 and
// obj+0x27C, then publishes the fully blended matrices to the per-bone
// scratch table (D_00275B40) and applies a post-process (func_00102760) to
// each of the first 3 words per bone matrix, before handing off to the clip
// arbiter anim_clip_arbiter.
extern float D_70003A20[4];
typedef struct Mat4 { unsigned char _b[0x40]; } Mat4;
extern int *D_00275B40;
extern Mat4 D_00286340;
extern Mat4 D_00287140;
extern Mat4 D_00287F40;
extern Mat4 D_00288D40;
extern short anim_slot_index(unsigned char *, int);
extern void bone_matrix_publish(unsigned char *, int, Mat4 *);
extern void func_00179CA0(Mat4 *, Mat4 *, Mat4 *, float);
extern void func_00102760(void *, void *);
extern int anim_clip_arbiter(unsigned char *, short, float, float);

void anim_matrix_dispatch(unsigned char *arg0)
{
    unsigned char *obj = (unsigned char *)arg0;
    int i;
    Mat4 *m1, *m2, *m3;
    Mat4 *p1, *p2;
    int j;
    int joff;
    int off;
    int k;
    int off2;
    int k2;

    if (*(float *)(obj + 0x278) <= 0.0f) {
        bone_matrix_publish(obj, anim_slot_index(obj, 2), &D_00287140);
    } else if (*(float *)(obj + 0x278) >= 1.0f) {
        bone_matrix_publish(obj, anim_slot_index(obj, 1), &D_00287140);
    } else {
        bone_matrix_publish(obj, anim_slot_index(obj, 0), &D_00288D40);
        if (*(float *)(obj + 0x278) <= 0.5f) {
            D_70003A20[0] = 0.5f - *(float *)(obj + 0x278);
            bone_matrix_publish(obj, anim_slot_index(obj, 2), &D_00287F40);
        } else {
            D_70003A20[0] = *(float *)(obj + 0x278) - 0.5f;
            bone_matrix_publish(obj, anim_slot_index(obj, 1), &D_00287F40);
        }
        m1 = &D_00287F40;
        m2 = &D_00288D40;
        m3 = &D_00287140;
        for (i = 0; i < obj[0xC]; i++) {
            func_00179CA0(m3, m2, m1, 2.0f * D_70003A20[0]);
            m1++;
            m2++;
            m3++;
        }
    }

    if (*(float *)(obj + 0x27C) <= 0.5f) {
        if (*(float *)(obj + 0x278) <= 0.0f) {
            bone_matrix_publish(obj, anim_slot_index(obj, 8), &D_00286340);
        } else if (*(float *)(obj + 0x278) >= 1.0f) {
            bone_matrix_publish(obj, anim_slot_index(obj, 7), &D_00286340);
        } else {
            bone_matrix_publish(obj, anim_slot_index(obj, 4), &D_00288D40);
            if (*(float *)(obj + 0x278) <= 0.5f) {
                D_70003A20[0] = 0.5f - *(float *)(obj + 0x278);
                bone_matrix_publish(obj, anim_slot_index(obj, 8), &D_00287F40);
            } else {
                D_70003A20[0] = *(float *)(obj + 0x278) - 0.5f;
                bone_matrix_publish(obj, anim_slot_index(obj, 7), &D_00287F40);
            }
            m1 = &D_00287F40;
            m2 = &D_00288D40;
            m3 = &D_00286340;
            for (i = 0; i < obj[0xC]; i++) {
                func_00179CA0(m3, m2, m1, 2.0f * D_70003A20[0]);
                m1++;
                m2++;
                m3++;
            }
        }
    } else {
        if (*(float *)(obj + 0x278) <= 0.0f) {
            bone_matrix_publish(obj, anim_slot_index(obj, 6), &D_00286340);
        } else if (*(float *)(obj + 0x278) >= 1.0f) {
            bone_matrix_publish(obj, anim_slot_index(obj, 5), &D_00286340);
        } else {
            bone_matrix_publish(obj, anim_slot_index(obj, 3), &D_00288D40);
            if (*(float *)(obj + 0x278) <= 0.5f) {
                D_70003A20[0] = 0.5f - *(float *)(obj + 0x278);
                bone_matrix_publish(obj, anim_slot_index(obj, 6), &D_00287F40);
            } else {
                D_70003A20[0] = *(float *)(obj + 0x278) - 0.5f;
                bone_matrix_publish(obj, anim_slot_index(obj, 5), &D_00287F40);
            }
            m1 = &D_00287F40;
            m2 = &D_00288D40;
            m3 = &D_00286340;
            for (i = 0; i < obj[0xC]; i++) {
                func_00179CA0(m3, m2, m1, 2.0f * D_70003A20[0]);
                m1++;
                m2++;
                m3++;
            }
        }
    }

    if (*(float *)(obj + 0x27C) <= 0.0f) {
        D_70003A20[0] = 0.5f;
    } else if (*(float *)(obj + 0x27C) >= 1.0f) {
        D_70003A20[0] = 0.5f;
    } else if (*(float *)(obj + 0x27C) <= 0.5f) {
        D_70003A20[0] = 0.5f - *(float *)(obj + 0x27C);
    } else {
        D_70003A20[0] = *(float *)(obj + 0x27C) - 0.5f;
    }

    p1 = &D_00286340;
    p2 = &D_00287140;
    for (k = 0, off = 0; k < obj[0xC]; k++) {
        func_00179CA0((Mat4 *)(*(int *)((char *)D_00275B40 + off) + 0x90), p2, p1, 2.0f * D_70003A20[0]);
        p1++;
        p2++;
        off += 4;
    }

    for (k2 = 0, off2 = 0; k2 < obj[0xC]; k2++) {
        for (j = 0, joff = 0; j < 3; j++) {
            unsigned char *node = (unsigned char *)*(int *)(off2 + (int)D_00275B40) + joff + 0x90;
            func_00102760(node, node);
            joff += 0x10;
        }
        off2 += 4;
    }

    obj[0x303] = 1;
    anim_clip_arbiter(obj, anim_slot_index(obj, 0), 0.0f, (float)*(short *)(obj + 0x276));
}
