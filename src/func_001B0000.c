// CFLAGS: -O4,p -sdatathreshold 4
typedef signed char        s8;
typedef unsigned char      u8;
typedef short              s16;
typedef unsigned short     u16;
typedef int                s32;
typedef unsigned int       u32;
typedef long long          s64;
typedef unsigned long long u64;
typedef float              f32;
typedef double             f64;

extern void func_001CB590(u8 *, s32, u8);
extern u8 D_0028B020[0x4680];

typedef struct Obj {
    char _pad[0x10];
    void (*fn10)(struct Obj *);
} Obj;
extern Obj *D_00275B44;

void func_001B0000(void) {
    u8 *p;
    s32 i;

    p = D_0028B020;
    i = 0;
    do {
        if (*p != 0) {
            u8 c = p[9];
            s32 stride = 0x2F0;
            func_001CB590(p, stride, c);
            D_00275B44->fn10(D_00275B44);
        }
        i += 1;
        p += 0x2F0;
    } while ((i < 0x18) & 0xFFFFu);
}
