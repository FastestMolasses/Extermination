// NEARMISS func_001F4A10  (vram 0x001F4A10, 0x1E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Remaining differences: the original inlines VU0 macro code (fog-quad load, the
// K-matrix row loads, the projection with Q divide, fog clamp and ftoi4) where this
// C calls the vu0_load_fog_quad / vu0_project stand-ins; inline asm is not approved
// for this function. Every scalar instruction outside that block matches, except that
// the key's store/reload through screen.z goes via a pointer register (a side effect
// of screen's address escaping into the stand-in call).
// Fixed 2026-10-27 (was 67.58%): -sdatathreshold 8 (the globals are gp-relative;
// D_0028A56C is an unsized array so it stays absolute), the random term written as
// a separate draw then base + r * base, the scale divided in place, the display slot
// re-read through one slot pointer after the calls, dir.w an int field, the packet
// passed as a sign-extended 64-bit value, the depth key kept in screen.z.
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

/* COP2 primitives; see src/func_001D72D0.c. */
extern void vu0_load_fog_quad(const void *quad);
extern void vu0_project(const Vec4 *matK, const Vec4 *p,
                        Vec4 *clip_out, IVec4 *screen_12_4);

void func_001F4A10(void *obj, const int *v)
{
    DVec4  dir;
    Vec4   pos;
    IVec4  screen;
    Vec4   clip;
    int   *slot;
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

    vu0_load_fog_quad(D_00275670 + 0xA0);
    vu0_project(D_70003AC0, &pos, &clip, &screen);

    /* screen.w is 16 * the GS FOG value, so this is 0xFFFFFF / F */
    screen.z = float_to_int(268435440.0f / (float)screen.w);

    func_001CB760(D_007635C0, screen.z, packet);
}
