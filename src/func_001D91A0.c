// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.05% NEARMISS).
// Fixes read from the original instructions:
//  - func_001028B8 takes three arguments (its definition and every other caller), not
//    four; func_00128250 returns unsigned int; the arg2 pass-through is an int pointer.
//  - Spelled like the byte-matched twin func_001F6210: each DMA tag block takes the ring
//    head q, then a body pointer tag = q + 0x10 right after it (before the head is
//    advanced), used for the type-5 body and the type-9 transform outputs; the 0x70003440 qwords
//    are copied through one src pointer.
//  - A redundant (void *) cast on the third func_001028B8 argument sets that argument
//    up first, as the original does.
//
// GIF/DMA packet builder. Builds a GIFtag + draw packet into the display
// list head held at D_00275670[0x1C], zeroes the 0x70003400 scratch region,
// packs an RGBA color from arg1's 4 floats via func_00128250, transforms
// vertices via func_001026D0, then dispatches a draw via the func_001D* set
// (branch on arg3 selects variant).
typedef int s128 __attribute__((mode(TI)));

extern char *D_00275670;
extern char D_70003400[];
extern char D_70003440[];
extern char D_70003470[];
extern s128 D_70003AC0[];
extern char D_0026E540[];

extern void func_001029C0(void *out);
extern void func_001026D0(void *out, void *a, void *b);
extern void func_001028B8(void *out, void *a, void *b);
extern unsigned int func_00128250(float arg);
extern void copy_qw4(s128 *dst, volatile s128 *src);
extern void func_001D1F80(int ch, int a, int b);
extern void func_001D4E20(int ch);
extern void func_001D4EB0(int ch);
extern void func_001D7080(int a0, int a1, float fa0);
extern void func_001D4F30(int ch, int *src);
extern void func_001D4B80(int ch, int *src);
extern void func_001D4C30(int ch, int *src);

void func_001D91A0(float *arg0, float *arg1, int *arg2, int arg3, float fp) {
    float sp60[16];
    float spA0[16];
    s128 spE0[4];
    float sp120[16];
    char *g;
    char *slot;
    char *q;
    char *tag;
    s128 *src;
    float scale;
    int color;

    scale = 0.1f * (fp / 20.0f);

    func_001029C0(spA0);
    spA0[0] = scale;
    spA0[5] = scale;
    spA0[10] = scale;
    func_001026D0(sp60, arg0, spA0);

    /* DMA tag block, type 5 */
    g = D_00275670;
    *(unsigned char *)(*(char **)(g + 0x1C) + 3) = 0x10;
    *(int *)(*(char **)(g + 0x1C) + 4) = 0;
    *(short *)(*(char **)(g + 0x1C) + 0) = 5;
    q = *(char **)(g + 0x1C);
    tag = q + 0x10;
    *(char **)(g + 0x1C) = q + 0x60;

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

    func_001028B8(D_70003470, arg1, (void *)D_0026E540);

    *(int *)(tag + 0) = 0x11000000;
    *(int *)(tag + 4) = 0x01000101;
    *(int *)(tag + 8) = 0;
    *(int *)(tag + 0xC) = 0x6C0403F5;
    src = (s128 *)D_70003440;
    *(s128 *)(tag + 0x10) = src[0];
    *(s128 *)(tag + 0x20) = src[1];
    *(s128 *)(tag + 0x30) = src[2];
    *(s128 *)(tag + 0x40) = src[3];

    /* DMA tag block, type 9 */
    g = D_00275670;
    slot = g + 0x1C;
    *(unsigned char *)(*(char **)slot + 3) = 0x10;
    *(int *)(*(char **)slot + 4) = 0;
    *(short *)(*(char **)slot + 0) = 9;
    q = *(char **)slot;
    tag = q + 0x10;
    *(char **)slot = q + 0xA0;
    *(s128 *)(q + 0x10) = 0;
    *(int *)(q + 0x14) = 0x01000101;
    *(int *)(q + 0x18) = 0;
    *(int *)(q + 0x1C) = 0x6C080000;
    func_001026D0(tag + 0x10, D_70003AC0, sp60);
    func_001026D0(tag + 0x50, D_70003400, sp60);

    color = func_00128250(arg1[3]) << 24;
    color |= func_00128250(arg1[2]) << 16;
    color |= func_00128250(arg1[1]) << 8;
    color |= func_00128250(arg1[0]);

    copy_qw4(spE0, D_70003AC0);
    func_001026D0(sp120, D_00275670 + 0x2380, sp60);
    func_001026D0(D_70003AC0, D_00275670 + 0x2340, sp120);

    if (arg3 == 0) {
        func_001D1F80(3, 2, 9);
        func_001D4E20(3);
        func_001D7080(3, color, 0.0f);
        func_001D4F30(3, arg2);
        func_001D4B80(3, arg2);
    } else {
        func_001D1F80(3, 2, 5);
        func_001D4EB0(3);
        func_001D7080(3, color, 0.0f);
        func_001D4F30(3, arg2);
        func_001D4C30(3, arg2);
    }

    copy_qw4(D_70003AC0, spE0);
}
