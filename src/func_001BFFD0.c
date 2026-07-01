// NEARMISS func_001BFFD0  (vram 0x001BFFD0, 0x308 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body/structure fully recovered (switch on state byte at e+4, 4 cases dispatched by mwcc in descending 3,2,1,0 order matching idiom-20). Sole residual: the switch selector `st` and the literal constant 3 are assigned to swapped argument registers (a2 vs a3) versus the target in the very first comp...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef long long s64;

extern void anim_advance_time(char *e, float f);
extern void bone_init_default_2(char *e, int a1);
extern void func_001AFC10(char *e);
extern int func_001B10B0(char *e, int a1, int a2);
extern void func_001B17A0(char *e);
extern void func_001BFF90(char *e, char *e2, u8 flag);
extern void func_001C68C0(char *e);
extern void func_001EFE00(int a0, char *e);
extern int D_00275638;
extern int D_0028A520[2];

void func_001BFFD0(char *e) {
    char *e0;
    char *e1;
    char *e2;
    u8 st;

    e0 = *(char **)(e + 0x20);
    st = *(u8 *)(e + 4);
    e2 = e + 0x1F0;
    e1 = e0 + 0x1F0;
    switch (st) {
    case 0:
        if (*(u8 *)(e0 + 4) == 3) {
            *(u8 *)(e + 4) = 3;
            return;
        }
        if (func_001B10B0(e, 0x20, 0x21) == 0) {
            bone_init_default_2(e, 0);
            *(int *)(e + 0x58) = D_0028A520[0];
            if (*(u8 *)(e + 0xD) == 1) {
                *(u8 *)(e + 0) = 1;
            } else {
                *(u8 *)(e + 0) = 3;
                *(int *)(*(char **)(e + 0x110) + 0x80) = 0xC0900000;
            }
            *(u8 *)(e + 4) = 1;
            *(s16 *)(e2 + 2) = 0;
            *(int **)(e + 0x30) = &D_00275638;
            *(int *)(e2 + 0x1C) = 0;
            *(s16 *)(e + 0x34) = 0x64;
        }
        break;
    case 1:
        if ((int)*(u8 *)(e0 + 4) >= 2) {
            *(u8 *)(e + 4) = 2;
        }
        if (*(u8 *)(e0 + 1) != 0) {
            if (*(u8 *)(e + 0xD) == 2) {
                if (*(s16 *)(e0 + 0x28) != 0) {
                    anim_advance_time(e, 1.0f);
                }
            } else {
                s32 t;
                anim_advance_time(e, 1.0f);
                if (*(s16 *)(e1 + 2) == 0) {
                    func_001BFF90(e, e2, 0);
                } else {
                    func_001BFF90(e, e2, 1);
                }
                t = *(s32 *)(e2 + 0x1C);
                if (t != 0) {
                    *(s32 *)(e2 + 0x1C) = t - 1;
                }
                {
                    s16 flags = *(s16 *)(e + 0x36);
                    if (flags != 0) {
                        if (*(u8 *)(e0 + 0) == 1) {
                            s64 wide = flags;
                            if (wide & 0x5000) {
                                if (*(s32 *)(e2 + 0x1C) == 0 && *(s32 *)(e1 + 0x1C) == 0) {
                                    if (wide & 0x4000) {
                                        func_001EFE00(0x80000027, e);
                                    }
                                    *(s32 *)(e2 + 0x1C) = 0x3C;
                                    *(s16 *)(e0 + 0x36) = *(s16 *)(e + 0x36);
                                    goto block_36;
                                }
                            } else {
                                *(s16 *)(e0 + 0x36) = flags;
block_36:
                                *(u8 *)(e0 + 0) = 3;
                            }
                        }
                        *(s16 *)(e + 0x36) = 0;
                        *(u8 *)(e + 0) = 1;
                    }
                }
            }
            func_001C68C0(e);
            func_001B17A0(e);
            (*(void (**)(char *))(e + 0x4C))(e);
        }
        break;
    case 2: {
        float loaded;
        float f;
        func_001BFF90(e, e2, 0);
        loaded = *(float *)(e + 0x64);
        f = loaded - 0.01f;
        *(float *)(e + 0x64) = f;
        if (f < 0.0f) {
            *(float *)(e + 0x64) = 0.0f;
            *(u8 *)(e + 4) = 3;
        }
        anim_advance_time(e, 1.0f);
        func_001C68C0(e);
        if (*(float *)(e + 0x64) != 0.0f) {
            (*(void (**)(char *))(e + 0x4C))(e);
        }
        break;
    }
    case 3:
        func_001AFC10(e);
        break;
    }
}
