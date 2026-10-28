// NEARMISS func_001CAAC0  (vram 0x001CAAC0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4); matching wave 2026-10-28.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Remaining differences:
// the scratch pointers for the projection come out with the vertex-copy and clip
// pointers in swapped temporaries (v0 and v1). The original evidently issued the
// projection as ONE asm statement whose operands are listed clip, source, screen;
// that form (build/agent_vC/caa_o8.c) is a 100% byte match but holds 14 instructions
// in one statement, which the leak guard rejects. Fixes so far: payload is the
// 64-bit value func_001CB760 takes (callers declare it as a pointer/int);
// -sdatathreshold 4; stack order position, screen, clip.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Depth-sorts a display-list node: copies the world position, projects it with the
// camera matrix (scratchpad 0x70003AC0) and the fog quad at render context + 0xA0,
// clamps the 12.4 depth key (negative -> 0xFFB000, below 0x1000 -> 0x1000) and files
// the payload into the depth-bucket page table D_007635C0 via func_001CB760. Returns
// the key.

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;
extern Vec4  D_70003AC0[4];
extern char  D_007635C0[];

extern void func_00102948(void *dst, const void *src);
extern void func_001CB760(int page_table, int key, unsigned long long payload);

int func_001CAAC0(const Vec4 *world_pos, unsigned long long payload)
{
    Vec4  pos;
    IVec4 screen;
    Vec4  clip;
    char *fog;
    const Vec4 *m;
    Vec4 *p;
    IVec4 *s;
    Vec4 *c;
    int   key;

    func_00102948(&pos, world_pos);
    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    c = &clip;
    p = &pos;
    s = &screen;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c));
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(s));

    key = screen.z;
    if (key < 0) {
        key = 0xFFB000;
    }
    if (key < 0x1000) {
        key = 0x1000;
    }
    func_001CB760((int)D_007635C0, key, payload);
    return key;
}
