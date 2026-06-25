// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state driver dispatched on the state byte at arg0+6 (switch 0..4).
// arg0 = self, arg1 = owner. State 0 bumps state, inits owner fields and
// kicks clip 0x2C. States 1/2/3 gate on the owner's 0x1000 input bit; state 3
// additionally eases the self+0x60 scalar toward 1.3f via func_001028B8 over a
// small per-frame delta (0x3BA3D70A). State 4 resets and rebuilds the bone.
// Tail-calls func_00132490(self, owner).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void bone_init_default_2(char *self, int n);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00132490(char *self, char *owner);
extern char *func_001EFE00(int a, char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern unsigned char D_0081070A;
extern char D_700038A0[];

void func_00131210(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x3C) = 0;
        *(float *)(arg1 + 0x34) = 1.0f;
        anim_clip_init(arg0, 0x2C, 5.0f, 0.0f);
        break;
    case 1:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            func_001FBD50(arg0, 0x7D9, 0, 300.0f);
            anim_clip_init(arg0, 0x2D, 1.0f, 0.0f);
        }
        break;
    case 2:
        *(char *)(arg1 + 0x6A) = 0x64;
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_001EFE00(0x8000002B, arg0);
            anim_clip_init(arg0, 0x2E, 1.0f, 0.0f);
            func_001FBD50(arg0, 0x7DA, 0, 300.0f);
        }
        break;
    case 3:
        *(char *)(arg1 + 0x6A) = 0x64;
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            if (*(float *)(arg0 + 0x60) == 1.3f) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init(arg0, 0x2F, 1.0f, 0.0f);
                func_001FBD50(arg0, 0x7DB, 0, 300.0f);
            }
        }
        *(int *)0x700038A0 = 0x3BA3D70A;
        *(int *)0x700038A4 = 0x3BA3D70A;
        *(int *)0x700038A8 = 0x3BA3D70A;
        *(int *)0x700038AC = 0;
        func_001028B8(arg0 + 0x60, D_700038A0, arg0 + 0x60);
        if (!(*(float *)(arg0 + 0x60) <= 1.3f)) {
            *(float *)(arg0 + 0x60) = 1.3f;
            *(float *)(arg0 + 0x64) = 1.3f;
            *(float *)(arg0 + 0x68) = 1.3f;
            *(float *)(arg0 + 0x6C) = 1.0f;
        }
        break;
    case 4:
        *(char *)(arg1 + 0x6A) = 0x64;
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            if (*(unsigned char *)(arg0 + 0xD) & 1) {
                if (D_0081070A != 0) {
                    *(short *)(arg0 + 0x34) = 0x12C;
                } else {
                    *(short *)(arg0 + 0x34) = 0xB4;
                }
            } else {
                if (D_0081070A != 0) {
                    *(short *)(arg0 + 0x34) = 0xFA;
                } else {
                    *(short *)(arg0 + 0x34) = 0x96;
                }
            }
            *(unsigned char *)(arg0 + 0xD) = *(unsigned char *)(arg0 + 0xD) | 0x80;
            *(char *)(arg1 + 0x6A) = 0;
            func_001FBD50(arg0, 0x7DC, 0, 300.0f);
            bone_init_default_2(arg0, 5);
        }
        break;
    }
    func_00132490(arg0, arg1);
}
