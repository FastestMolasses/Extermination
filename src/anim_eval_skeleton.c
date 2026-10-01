// NEARMISS anim_eval_skeleton  (vram 0x001C6DA0, 0x680 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 11.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Hand-written VU0 macro code in the original (row scaling and the multiply-accumulate matrix
// products, inlined twice); the C states the arithmetic with float loops and two static helpers,
// so only the call sequence lines up.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md. First-level lane 2026-10-01
// (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Evaluates an actor's skeleton into world matrices (port: em_pose_host_
// 001C6DA0, ANIM_RUNTIME_REST.md). The actor's root matrix +0xD0 is built
// from its position +0xB0, rotation +0xC0 and scale +0x60
// (build_trs_matrix). Each bone node (the +0x110 pointer array, +0xC bones)
// gets a local basis L in the scratchpad (0x70003440): the identity rotated
// by the node's Euler angles +0x70 (func_00102C58), translation +0x7C/+0x80/
// +0x84 in row 3, rows 0..2 scaled by the halfwords +0x88/+0x8A/+0x8C / 4096;
// then L = R x L with the channel rotation R (0x70003400), and the node's
// world matrix +0x90 = P x L with P the parent's world matrix (parent index
// halfword +0x64, -1 = the root matrix). Bone 0 uses R = identity; the other
// bones build R from their two key quaternions +0x30 / +0x40 blended by +0x50
// (quat_nlerp into 0x70003600, quat_to_mat3 with the node's translation +0),
// rows 0..2 scaled by +0x18 / +0x1C / +0x20.
// The row scaling and the matrix products are hand-written VU0 macro code
// in the original (multiply-accumulate of the left rows by the right row's
// lanes); here they are plain float loops.
extern void build_trs_matrix(float *m, float *pos, float *rot, float *scale);
extern void func_001029C0(float *m);
extern void func_00102C58(float *dst, float *src, float *angles);
extern void quat_nlerp(float *out, float *a, float *b, float t);
extern void quat_to_mat3(float *m, float *q, char *node);

static void mat_mul(float *out, float *left, float *right) {
    float l[16];
    int i;
    int j;

    for (i = 0; i < 16; i++) {
        l[i] = left[i];
    }
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            out[j * 4 + i] = l[i] * right[j * 4] + l[4 + i] * right[j * 4 + 1]
                           + l[8 + i] * right[j * 4 + 2] + l[12 + i] * right[j * 4 + 3];
        }
    }
}

static void node_world(char *a, char *node) {
    float *L = (float *)0x70003440;
    float *R = (float *)0x70003400;
    float s;
    int k;
    int i;
    short parent;

    func_001029C0(L);
    func_00102C58(L, L, (float *)(node + 0x70));
    L[12] = *(float *)(node + 0x7C);
    L[13] = *(float *)(node + 0x80);
    L[14] = *(float *)(node + 0x84);
    for (k = 0; k < 3; k++) {
        s = (float)*(short *)(node + 0x88 + k * 2) * (1.0f / 4096.0f);
        for (i = 0; i < 3; i++) {
            L[k * 4 + i] *= s;
        }
    }
    mat_mul(L, R, L);
    parent = *(short *)(node + 0x64);
    if (parent != -1) {
        mat_mul((float *)(node + 0x90), (float *)(((char **)(a + 0x110))[parent] + 0x90), L);
    } else {
        mat_mul((float *)(node + 0x90), (float *)(a + 0xD0), L);
    }
}

void anim_eval_skeleton(char *a) {
    float *R = (float *)0x70003400;
    char *node;
    int k;
    int i;
    int b;

    build_trs_matrix((float *)(a + 0xD0), (float *)(a + 0xB0), (float *)(a + 0xC0), (float *)(a + 0x60));
    node = ((char **)(a + 0x110))[0];
    func_001029C0(R);
    node_world(a, node);
    for (b = 1; b < *(unsigned char *)(a + 0xC); b++) {
        node = ((char **)(a + 0x110))[b];
        quat_nlerp((float *)0x70003600, (float *)(node + 0x30), (float *)(node + 0x40), *(float *)(node + 0x50));
        quat_to_mat3(R, (float *)0x70003600, node);
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 3; i++) {
                R[k * 4 + i] *= *(float *)(node + 0x18 + k * 4);
            }
        }
        node_world(a, node);
    }
}
