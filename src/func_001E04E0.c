// NEARMISS func_001E04E0  (vram 0x001E04E0, 0x264 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 58.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two stacked walls; logic is fully recovered (setup calls, packet header/GIFtag layout, both loop bounds and shapes, the acc thread, teardown and kick all reproduce instruction-for-instruction where the compiler cooperates). (1) VU0 wall: after func_001CD370(0) the target executes four `lqc2 vf24/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// SEMANTICS: draws a 16x16 vertex grid as 15 triangle strips into the display
// list held in the render context D_00275670 (a gp-relative pointer; word
// +0x1C is the running DMA/display-list write cursor).
//   arg0 = transform context handed to func_001028B8 (per-vertex transform)
//   arg1 = passed through to func_001E0240 (per-vertex emit)
//   t    = float interpolation/morph parameter handed to func_00103230
//   D_0081C0F0 = the source vertex grid: 16 bytes per vertex, 0x100 per row
// Sequence:
//   - save the current write cursor (wp0) for the final kick;
//   - render-state setup on unit 3 with texture buffer 0x258000:
//       func_001D6B10(3, 0x258000, 8, 8), func_001D1F80(3, 2, 1),
//       func_001D6BA0(3, 0x258000, 8, 8, 0, 0), func_001D1FF0(3, 3);
//   - func_001CD370(0) returns the base of a 4-quadword (4x4) matrix. The
//     ORIGINAL then loads its four rows into the VU0 registers vf24..vf27 with
//     four `lqc2 vf24..vf27, 0x0/0x10/0x20/0x30($v0)` instructions, which
//     func_001E0240 consumes. There is no readable-C construct that emits
//     `lqc2` into a fixed vf register (every matched lqc2 function in this
//     project is asm-void), so those four instructions are the one piece of
//     this function that cannot be expressed here — the call is kept so the
//     matrix fetch and its ordering stay visible.
//   - per row i (0..14) open a 0x630-byte packet at the cursor: header byte
//     [3] = 0x10, word [4] = 0, halfword [0] = 0x62; advance the cursor by
//     0x630; zero the quadword at +0x10; control word 0x50000061 at +0x1C;
//     GIFtag 0x302E400000008020 at +0x20 and 0x421 at +0x28; vertex data
//     begins at +0x30;
//   - per column j (0..15) emit the two strip vertices (row i then row i+1):
//     morph the source vertex by t (func_00103230), transform it
//     (func_001028B8), then emit it (func_001E0240), which threads a running
//     accumulator `acc` and consumes 0x30 bytes of the packet per vertex;
//   - teardown: func_001D1F20(3), func_001D1FF0(3, 1), close the list (byte
//     [3] = 0x60, word [4] = 0, halfword [0] = 0), advance the cursor by 0x10,
//     and kick with func_001CAAC0(arg0, wp0, D_00275670).
//
typedef int u128 __attribute__((mode(TI)));

extern char *D_00275670;
extern char D_0081C0F0[];

extern void func_001D6B10(int, int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D6BA0(int, int, int, int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D1F20(int);
extern char *func_001CD370(int);
extern void func_00103230(float *, char *, float);
extern void func_001028B8(float *, int, float *);
extern int func_001E0240(char *, float *, int, int);
extern void func_001CAAC0(int, char *, char *);

void func_001E04E0(int arg0, int arg1, float t)
{
    char *wp0;
    char *p;
    char *dl;
    int i;
    int j;
    int acc;
    float v[4];

    wp0 = *(char **)(D_00275670 + 0x1C);
    func_001D6B10(3, 0x258000, 8, 8);
    func_001D1F80(3, 2, 1);
    func_001D6BA0(3, 0x258000, 8, 8, 0, 0);
    func_001D1FF0(3, 3);
    /* the original also does: lqc2 vf24..vf27, 0x0/0x10/0x20/0x30(result) */
    func_001CD370(0);

    for (i = 0; i < 15; i++) {
        *(char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x10;
        *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
        *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 0x62;
        p = *(char **)(D_00275670 + 0x1C);
        *(char **)(D_00275670 + 0x1C) = p + 0x630;
        *(u128 *)(p + 0x10) = 0;
        *(int *)(p + 0x1C) = 0x50000061;
        *(long long *)(p + 0x20) = 0x8020 | ((long long)0x302E4000 << 32);
        *(long long *)(p + 0x28) = 0x421;
        dl = p + 0x10;
        dl += 0x20;
        acc = 0;
        for (j = 0; j < 16; j++) {
            func_00103230(v, &D_0081C0F0[(i << 8) + (j << 4)], t);
            func_001028B8(v, arg0, v);
            acc = func_001E0240(dl, v, arg1, acc);
            dl += 0x30;
            func_00103230(v, &D_0081C0F0[((i + 1) << 8) + (j << 4)], t);
            func_001028B8(v, arg0, v);
            acc = func_001E0240(dl, v, arg1, acc);
            dl += 0x30;
        }
    }

    func_001D1F20(3);
    func_001D1FF0(3, 1);
    *(char *)(*(char **)(D_00275670 + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
    *(short *)(*(char **)(D_00275670 + 0x1C) + 0) = 0;
    *(char **)(D_00275670 + 0x1C) = *(char **)(D_00275670 + 0x1C) + 0x10;
    func_001CAAC0(arg0, wp0, D_00275670);
}
