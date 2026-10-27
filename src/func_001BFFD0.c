// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 97.24% NEARMISS).
// Fixes read from the original instructions:
// - The old residual note (swapped switch-selector registers) was wrong; the real
//   difference was in state 1's flag hand-off: the original stores the partner's
//   state byte (3) separately in each of the two arms, not through a shared goto
//   label. Writing the store in both arms reproduces the code.
//
// Per-frame state machine for an entity paired with a partner (e+0x20): the state
// byte at e+4 runs 0 (wait/init), 1 (active, animate and pass flags to the
// partner), 2 (fade out the value at e+0x64), 3 (finish via func_001AFC10).

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
                                    *(u8 *)(e0 + 0) = 3;
                                }
                            } else {
                                *(s16 *)(e0 + 0x36) = flags;
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
