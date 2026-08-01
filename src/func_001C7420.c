// NEARMISS func_001C7420  (vram 0x001C7420, 0x4E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 4.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char  u8;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; }        Mtx4;   /* rows; r[3] is the translation row */

/* EE scratchpad (0x70000000) working set used by this uploader. */
#define SPR_MTX_A  ((Mtx4 *)0x70003400)   /* light / normal-space matrix           */
#define SPR_MTX_B  ((Mtx4 *)0x70003440)   /* object matrix, then reused as scratch */
#define SPR_MTX_C  ((Mtx4 *)0x70003480)   /* orthonormalised bone matrix scratch   */
#define SPR_MTX_VP ((Mtx4 *)0x70003AC0)   /* view-projection                       */

extern char *D_00275670;                  /* renderer context block (gp-relative)  */
extern void  func_001D89D0(char *actor, int mtx_a, char *mtx_b, int vel);

/* out = a * b, row-vector convention (see header). */
static void mtx_mul(Mtx4 *out, const Mtx4 *a, const Mtx4 *b)
{
    int i;
    for (i = 0; i < 4; i++) {
        out->r[i].x = a->r[i].x * b->r[0].x + a->r[i].y * b->r[1].x
                    + a->r[i].z * b->r[2].x + a->r[i].w * b->r[3].x;
        out->r[i].y = a->r[i].x * b->r[0].y + a->r[i].y * b->r[1].y
                    + a->r[i].z * b->r[2].y + a->r[i].w * b->r[3].y;
        out->r[i].z = a->r[i].x * b->r[0].z + a->r[i].y * b->r[1].z
                    + a->r[i].z * b->r[2].z + a->r[i].w * b->r[3].z;
        out->r[i].w = a->r[i].x * b->r[0].w + a->r[i].y * b->r[1].w
                    + a->r[i].z * b->r[2].w + a->r[i].w * b->r[3].w;
    }
}

/* vmul.xyz / vaddy.x / vaddz.x / vsqrt / vdiv / vmulq.xyz on one row.
   This is the VU divide unit, NOT a libm call: vsqrt writes $Q and the code
   stalls on vwaitq. The original leaves w = 0 (it clears the destination
   register with vsub.xyzw $vf6,$vf0,$vf0 and only writes xyz back), and it does
   NOT guard against a zero-length row — the VU divide unit yields its saturated
   value and sets the D flag instead of trapping. */
static void normalize_xyz(Vec4 *dst, const Vec4 *src)
{
    float len = __builtin_sqrtf(src->x * src->x + src->y * src->y + src->z * src->z);
    float inv = 1.0f / len;
    dst->x = src->x * inv;
    dst->y = src->y * inv;
    dst->z = src->z * inv;
    dst->w = 0.0f;
}

char *func_001C7420(char *actor, int vuaddr, int chan)
{
    char **cursor;      /* &D_00275670[chan*4 + 0x10] : the channel's DL cursor  */
    char  *first;       /* value of that cursor on entry — the return value      */
    char  *tag;         /* the DMAtag qword currently being written              */
    Mtx4  *dst;         /* where the next pair of bone matrices goes             */
    int    remaining;   /* quadwords still to upload (2 matrices == 8 per bone)  */
    int    chunk;       /* quadwords in the packet being built                   */
    int    vu;          /* running VU1 destination address                       */
    int    bone;        /* bone index                                            */
    int    n;

    cursor = (char **)(D_00275670 + chan * 4 + 0x10);
    first  = *cursor;

    /* 1. Build the object matrix (SPR_MTX_B) and the normal matrix (SPR_MTX_A). */
    func_001D89D0(actor, (int)SPR_MTX_A, (char *)SPR_MTX_B, (int)(actor + 0x80));

    /* 2. Packet #0 — upload the object matrix to VU1 address `vuaddr`. */
    tag = *cursor;
    tag[3] = 0x10;                                  /* DMAtag id 1 = CNT         */
    *(int *)(tag + 4) = 0;
    *(short *)(tag + 0) = 5;                        /* qwc: 1 VIF quad + 4 data  */
    *cursor = tag + 0x60;

    *(int *)(tag + 0x10) = 0;                       /* VIFNOP                    */
    *(int *)(tag + 0x14) = 0x11000000;              /* FLUSH                     */
    *(int *)(tag + 0x18) = 0x01000101;              /* STCYCL cl=1 wl=1          */
    *(int *)(tag + 0x1C) = 0x6C040000 | vuaddr;     /* UNPACK V4-32 num=4        */
    *(Mtx4 *)(tag + 0x20) = *SPR_MTX_B;

    /* 3. SPR_MTX_B is now free; zero it. */
    SPR_MTX_B->r[0].x = SPR_MTX_B->r[0].y = SPR_MTX_B->r[0].z = SPR_MTX_B->r[0].w = 0.0f;
    SPR_MTX_B->r[1].x = SPR_MTX_B->r[1].y = SPR_MTX_B->r[1].z = SPR_MTX_B->r[1].w = 0.0f;
    SPR_MTX_B->r[2].x = SPR_MTX_B->r[2].y = SPR_MTX_B->r[2].z = SPR_MTX_B->r[2].w = 0.0f;
    SPR_MTX_B->r[3].x = SPR_MTX_B->r[3].y = SPR_MTX_B->r[3].z = SPR_MTX_B->r[3].w = 0.0f;

    remaining = (int)*(u8 *)(actor + 0x0C) * 8;     /* 8 qwords == 2 mtx / bone  */
    bone = 0;
    vu   = 0;

    /* 4. One packet per <=248-quadword chunk. */
    do {
        chunk = (remaining < 0xF9) ? remaining : 0xF8;

        tag = *cursor;
        tag[3] = 0x10;                              /* DMAtag id 1 = CNT         */
        *(int *)(tag + 4) = 0;
        *(short *)(tag + 0) = (short)(chunk + 1);   /* qwc: 1 VIF quad + chunk   */
        *cursor = tag + (chunk + 2) * 0x10;

        *(int *)(tag + 0x10) = 0;                   /* VIFNOP                    */
        *(int *)(tag + 0x14) = 0;                   /* VIFNOP (no FLUSH here)    */
        *(int *)(tag + 0x18) = 0x01000101;          /* STCYCL cl=1 wl=1          */
        *(int *)(tag + 0x1C) = 0x6C000000 | (chunk << 16) | vu;  /* UNPACK V4-32 */

        dst = (Mtx4 *)(tag + 0x20);
        vu        += chunk;
        remaining -= chunk;

        /* 5. Two matrices per bone. */
        for (n = chunk; n != 0; n -= 8, dst += 2, bone++) {
            Mtx4 *node = (Mtx4 *)(*(char **)(actor + 0x110 + bone * 4) + 0x90);

            if (*(short *)(actor + 0x94) == bone) {
                /* Collapsed bone: keep only the translation row. */
                SPR_MTX_B->r[3] = node->r[3];
                mtx_mul(&dst[0], SPR_MTX_B, SPR_MTX_VP);
                mtx_mul(&dst[1], SPR_MTX_B, SPR_MTX_A);
            } else {
                mtx_mul(&dst[0], node, SPR_MTX_VP);

                normalize_xyz(&SPR_MTX_C->r[0], &node->r[0]);
                normalize_xyz(&SPR_MTX_C->r[1], &node->r[1]);
                normalize_xyz(&SPR_MTX_C->r[2], &node->r[2]);
                SPR_MTX_C->r[3] = node->r[3];

                mtx_mul(&dst[1], SPR_MTX_C, SPR_MTX_A);
            }
        }
    } while (remaining != 0);

    return first;
}
