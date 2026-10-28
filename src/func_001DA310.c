// NEARMISS func_001DA310  (vram 0x001DA310, 0x384 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.09% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). Body and
// control flow are faithful (spelled like the byte-matched twin func_001D91A0).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// NEARMISS 99.09 (matching wave 2026-10-27, was 93.21). Fixes: func_001028B8 takes three
// arguments; func_00102C58's third argument is a float pointer (its definition); the
// stack vectors are float arrays with float constant stores; each DMA tag block keeps
// tag = q + 0x10 like the twin; the 0x70003440 qwords copy through one src pointer.
// Remaining differences: the 2.3.3 permuter found that reading the first 0x70003440
// qword straight from the symbol (the other three through src) gives the original's
// register colouring for the whole type-5/type-9 tag stretch, but then the first qword
// load rematerialises the symbol address instead of using the shared src base (one
// extra address pair). Writing all four through src gives the shared base but colours
// the 0x01000101 word (shared by both tag blocks) into a different temporary (98.87%).
// The twin func_001D91A0 gets both with the all-src text, so the trigger is elsewhere.
//
// GIF/DMA packet builder (sibling of func_001D91A0). Builds a scaled transform
// from arg0's qword and fparg0, composes it via func_00102918/func_00102C58,
// writes a GIFtag + draw packet into the display-list head at D_00275670[0x10],
// zeroes the 0x70003400 scratch region, packs an RGBA color from arg2's 4
// floats via func_00128250, then dispatches the draw (func_001D1F80/...).
typedef int s128 __attribute__((mode(TI)));

extern char *D_00275670;
extern char D_70003400[];
extern char D_70003440[];
extern char D_70003470[];
extern s128 D_70003AC0[];
extern char D_0026E610[];

extern void func_001029C0(void *out);
extern void func_001026D0(void *out, void *a, void *b);
extern void func_001028B8(void *out, void *a, void *b);
extern void func_00102918(void *out, void *a, void *v);
extern void func_00102C58(void *out, void *a, float *v);
extern unsigned int func_00128250(float arg);
extern void copy_qw4(s128 *dst, volatile s128 *src);
extern void func_001D1F80(int ch, int a, int b);
extern void func_001D4EA0(void);
extern void func_001D7080(int a0, int a1, float fa0);
extern void func_001D4FB0(int arg);
extern void func_001D4C20(int arg);

void func_001DA310(s128 *arg0, float *arg1, float *arg2, int arg3, float fp) {
    float sp50[16];
    float sp90[16];
    float spD0[4];
    float spE0[4];
    s128 spF0[4];
    float sp130[16];
    char *g;
    char *q;
    char *tag;
    s128 *src;
    float scale;
    int color;

    *(s128 *)spD0 = *arg0;
    spD0[3] = 1.0f;

    func_001029C0(sp50);
    func_001029C0(sp90);
    scale = 0.1f * fp;
    sp90[0] = scale;
    sp90[5] = 4.0f;
    sp90[10] = scale;
    func_001026D0(sp50, sp90, sp50);

    spE0[0] = 0.0f;
    spE0[1] = -20.0f;
    spE0[2] = 0.0f;
    spE0[3] = 1.0f;
    func_00102918(sp50, sp50, spE0);
    func_00102C58(sp50, sp50, arg1);
    func_00102918(sp50, sp50, spD0);

    /* DMA tag block, type 5 */
    g = D_00275670;
    *(unsigned char *)(*(char **)(g + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(g + 0x10) + 4) = 0;
    *(short *)(*(char **)(g + 0x10) + 0) = 5;
    q = *(char **)(g + 0x10);
    tag = q + 0x10;
    *(char **)(g + 0x10) = q + 0x60;

    *(int *)0x70003400 = 0;
    *(int *)0x70003404 = 0;
    *(int *)0x70003408 = 0;
    *(int *)0x7000340C = 0;
    *(int *)0x70003410 = 0;
    *(int *)0x70003414 = 0;
    *(int *)0x70003418 = 0;
    *(int *)0x7000341C = 0;
    *(int *)0x70003420 = 0;
    *(int *)0x70003424 = 0;
    *(int *)0x70003428 = 0;
    *(int *)0x7000342C = 0;
    *(int *)0x70003430 = 0;
    *(int *)0x70003434 = 0;
    *(int *)0x70003438 = 0;
    *(int *)0x7000343C = 0;
    *(int *)0x70003440 = 0;
    *(int *)0x70003444 = 0;
    *(int *)0x70003448 = 0;
    *(int *)0x7000344C = 0;
    *(int *)0x70003450 = 0;
    *(int *)0x70003454 = 0;
    *(int *)0x70003458 = 0;
    *(int *)0x7000345C = 0;
    *(int *)0x70003460 = 0;
    *(int *)0x70003464 = 0;
    *(int *)0x70003468 = 0;
    *(int *)0x7000346C = 0;

    func_001028B8(D_70003470, arg2, (void *)D_0026E610);

    *(int *)(tag + 0) = 0x11000000;
    *(int *)(tag + 4) = 0x01000101;
    *(int *)(tag + 8) = 0;
    *(int *)(tag + 0xC) = 0x6C0403F5;
    src = (s128 *)D_70003440;
    *(s128 *)(tag + 0x10) = ((s128 *)D_70003440)[0];
    *(s128 *)(tag + 0x20) = src[1];
    *(s128 *)(tag + 0x30) = src[2];
    *(s128 *)(tag + 0x40) = src[3];

    /* DMA tag block, type 9 */
    g = D_00275670;
    *(unsigned char *)(*(char **)(g + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(g + 0x10) + 4) = 0;
    *(short *)(*(char **)(g + 0x10) + 0) = 9;
    q = *(char **)(g + 0x10);
    tag = q + 0x10;
    *(char **)(g + 0x10) = q + 0xA0;
    *(s128 *)(q + 0x10) = 0;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C080000;
    func_001026D0(tag + 0x10, D_70003AC0, sp50);
    func_001026D0(tag + 0x50, D_70003400, sp50);

    color = func_00128250(arg2[3]) << 24;
    color |= func_00128250(arg2[2]) << 16;
    color |= func_00128250(arg2[1]) << 8;
    color |= func_00128250(arg2[0]);

    copy_qw4(spF0, D_70003AC0);
    func_001026D0(sp130, D_00275670 + 0x2380, sp50);
    func_001026D0(D_70003AC0, D_00275670 + 0x2340, sp130);

    func_001D1F80(0, 2, 9);
    func_001D4EA0();
    func_001D7080(0, color, 0.0f);
    func_001D4FB0(arg3);
    func_001D4C20(arg3);

    copy_qw4(D_70003AC0, spF0);
}
