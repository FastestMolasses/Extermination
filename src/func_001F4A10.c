// NEARMISS func_001F4A10  (vram 0x001F4A10, 0x1E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 80.55%): the fog-quad load, K-matrix row loads, projection with Q divide, fog clamp and 12.4 conversion are the original inline VU0 code, written as one-instruction asm statements; the K pointer and the pos/clip/screen addresses are locals set before the VU0 block (the original computes them ahead of it).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Remaining differences: register names of the three stack addresses before the VU0 block (original pos/screen/clip in v1/a2/v0, mwcc picks v0/a0/v1); declaration and assignment orders and an inline-helper form did not change it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

typedef unsigned char u8;
typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;
typedef struct { float x, y, z; int w; } DVec4;

extern char *D_00275670;        /* per-frame render context        */
extern char *D_00275B44;        /* current subject object          */
extern Vec4  D_70003AC0[4];     /* camera K = P*V, EE scratchpad   */
extern char  D_007635C0[];      /* depth-bucket page table         */
extern void *D_0028A56C[];

extern int   func_00122BB8(void);                    /* LCG, 31-bit positive */
extern void  func_001C7900(void *a0, Vec4 *v, int vu_addr, int slot);
extern void *func_001C6120(void *table, int index);
extern void  func_001D3990(void *entry);
extern void  func_00102948(void *dst, const void *src);
extern int   float_to_int(float v);
extern void  func_001CB760(void *page_table, int key, unsigned long long payload);

void func_001F4A10(void *obj, const int *v)
{
    DVec4  dir;
    Vec4   pos;
    IVec4  screen;
    Vec4   clip;
    int   *slot;
    Vec4  *k;
    Vec4  *pp;
    IVec4 *ps;
    Vec4  *pc;
    long long packet;
    float  base, scale;

    packet = *(int *)(D_00275670 + 0x1C);       /* display slot 3 */

    base  = (float)v[3] / 128.0f;
    scale = (float)func_00122BB8() / 2147483648.0f;
    scale = base + scale * base;
    scale /= 32.0f;

    dir.x = (float)v[0] * scale;
    dir.y = (float)v[1] * scale;
    dir.z = (float)v[2] * scale;
    dir.w = 0;

    func_001C7900(obj, (Vec4 *)&dir, 0x3F5, 3);
    func_001D3990(func_001C6120(D_0028A56C[0], 0xC));

    /* chain terminator: DMAtag id 6 (RET), QWC 0 */
    slot = (int *)(D_00275670 + 0x1C);
    ((u8 *)*slot)[3] = 0x60;
    *(int *)((u8 *)*slot + 4) = 0;
    *(short *)((u8 *)*slot + 0) = 0;
    *slot = *slot + 0x10;

    func_00102948(&pos, D_00275B44 + 0xB0);

    /* VU0: load the fog parameters (render context +0xA0) and the camera
       matrix K into VU0, transform pos by K, keep the homogeneous clip
       position, then divide by w, compute the fog value from w (scaled,
       offset and clamped by the fog parameters) and convert x, y, z and
       fog to 12.4 fixed point */
    asm("lqc2 vf23, 0x0(%0)" : : "r"(D_00275670 + 0xA0));
    k = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(k));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(k));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(k));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(k));
    pp = &pos;
    pc = &clip;
    ps = &screen;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pp));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(pc));
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(ps));

    /* screen.w is 16 * the GS FOG value, so this is 0xFFFFFF / F */
    screen.z = float_to_int(268435440.0f / (float)screen.w);

    func_001CB760(D_007635C0, screen.z, packet);
}
