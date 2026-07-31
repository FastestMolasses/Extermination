// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// fx_render: walks the func_001F5CA0() particle/FX record list (0x28-byte records,
// terminated by a negative halfword at +0) and, for each record, builds its world
// transform (identity via func_001029C0, then translate by rec+0x18..0x20 and scale
// by rec+0xC..0x14), resolves the record's colour row by linear-searching the
// 17-entry D_0025CA40[] table for (((D_00810700<<8)+D_00810701)<<8) + rec->id,
// applies a random blend jitter from func_00122BB8() to the resolved colour, then
// emits three DMA tag blocks (types 5, 9 and 0) into the display-list ring at
// D_00275670->+0x1C before dispatching func_001D3D90 and func_001CAAC0.
//
extern char *func_001F5CA0(void);
extern void func_001D8C20(int);
extern int func_001C6120(int, short);
extern void func_001029C0(void *out);
extern void func_00102C58(void *out, void *a, void *b);
extern void func_00102918(void *out, void *a, void *b);
extern void func_00102948(void *out, void *a);
extern int func_00122BB8(void);
extern void func_001028B8(void *out, void *a, void *b);
extern void func_001026D0(void *out, void *a, void *b);
extern void func_001D3D90(int);
extern void func_001CAAC0(void *out, char *a, char *b);

typedef int s128 __attribute__((mode(TI)));

typedef struct { float x, y, z; int w; } VEC;

extern char *D_00275670;
extern int D_0028A59C[];
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern char D_0025CA40[];
extern char D_0026EB20[];
extern char D_0026EB60[];
extern char D_70003400[];
extern char D_70003440[];
extern char D_70003470[];
extern char D_70003AC0[];

void func_001F6210(void) {
    char *rec = func_001F5CA0();
    char *slot;
    int base = ((D_00810700[0] << 8) + D_00810701[0]) << 8;

    if (rec == 0) {
        return;
    }
    func_001D8C20(1);
    while (*(short *)(rec + 0) >= 0) {
        VEC sp70;
        VEC sp80;
        float sp90[4];
        float spA0[16];
        char *g;
        char *q;
        s128 *src;
        unsigned int i;
        int id;
        int handle;
        char *owner;
        int key;
        char *tag;

        g = D_00275670;
        id = D_0028A59C[0];
        owner = *(char **)(g + 0x1C);
        handle = func_001C6120(id, (short)*(short *)(rec + 4));

        sp80.x = *(float *)(rec + 0x18);
        sp80.y = *(float *)(rec + 0x1C);
        sp80.z = *(float *)(rec + 0x20);
        sp80.w = 0;
        sp70.x = *(float *)(rec + 0xC);
        sp70.y = *(float *)(rec + 0x10);
        sp70.z = *(float *)(rec + 0x14);
        sp70.w = 0x3F800000;

        func_001029C0(spA0);
        func_00102C58(spA0, spA0, &sp80);
        func_00102918(spA0, spA0, &sp70);

        key = *(short *)(rec + 4) + base;
        func_00102948(sp90, D_0026EB20);
        i = 0;
        while (1) {
            if (key == *(int *)(D_0025CA40 + i * 8)) {
                func_00102948(sp90, D_0026EB20 + (*(short *)(D_0025CA40 + i * 8 + 4) * 0x10));
                break;
            }
            i += 1;
            if (i >= 0x11U) {
                break;
            }
        }

        {
            float negaz;
            float az;
            float t;
            az = sp90[3];
            negaz = -az;
            t = negaz + ((az - negaz) * (4.656613e-10f * (float)func_00122BB8()));
            sp90[0] += sp90[0] * t;
            sp90[1] += sp90[1] * t;
            sp90[2] += sp90[2] * t;
            sp90[3] = 0.0f;
        }

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

        func_001028B8(D_70003470, sp90, D_0026EB60);

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
        *(char **)slot = q + 0xA0;
        *(s128 *)(q + 0x10) = 0;
        *(int *)(q + 0x14) = 0x01000101;
        *(int *)(q + 0x18) = 0;
        tag = q + 0x10;
        *(int *)(q + 0x1C) = 0x6C080000;
        func_001026D0(tag + 0x10, D_70003AC0, spA0);
        func_001026D0(tag + 0x50, D_70003400, spA0);

        func_001D3D90(handle);

        /* DMA tag block, type 0 (terminator) */
        g = D_00275670;
        *(unsigned char *)(*(char **)(g + 0x1C) + 3) = 0x60;
        *(int *)(*(char **)(g + 0x1C) + 4) = 0;
        *(short *)(*(char **)(g + 0x1C) + 0) = 0;
        *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;

        func_001CAAC0(&sp70, owner, g);
        rec += 0x28;
    }
    func_001D8C20(0);
}
