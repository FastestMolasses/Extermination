// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Actor sub-state tick: spawn/clip setup (0), activation-flag wait (1), scripted-exit
// handshake (2), then the common per-frame animation advance + post-update callback.
// The `zi = 0; z = (float)zi;` staging is load-bearing: it is what makes mwcc schedule
// `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12` at the case-0 anim_clip_init call.
extern void anim_clip_init(int self, int clip, float a, float b);
extern int anim_advance_time(int self, float t);
extern int func_00122BB8(int self, int a);
extern void func_00131ED0(int self);
extern void func_001B17A0(int self);
extern void func_001B5360(int self);
extern void func_001C68C0(int self);
extern char *func_001EFE00(int a, int self);
extern void func_001FBD50(int self, int a, int b, float f);
extern short D_0028A9A0;
extern char D_0028A9A2;
extern char D_0081080F;

void func_00137C80(int arg0, int arg1) {
    unsigned char st;
    int zi;
    float z;

    st = *(unsigned char *)(arg0 + 5);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 5) = st + 1;
        *(char *)(arg0 + 0xD) = 0;
        *(float *)(arg1 + 0x28) = 1.0f;
        if (*(short *)(arg0 + 0x34) != 0) {
            zi = 0;
            z = (float)zi;
            anim_clip_init(arg0, 2, 5.0f, z);
            func_001FBD50(arg0, 0x89A, 0, 500.0f);
        } else {
            D_0081080F = 1;
            anim_clip_init(arg0, 4, 5.0f, 0.0f);
            *(char **)(arg0 + 0x20) = func_001EFE00(0x80000046, arg0);
            func_001FBD50(arg0, 0x89B, 0, 500.0f);
            func_001FBD50(arg0, 0x89C, 0, 500.0f);
        }
        break;
    case 1:
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(short *)(arg0 + 0x34) != 0) {
                *(char *)(arg0 + 0) = 1;
                *(char *)(arg0 + 4) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
                *(char *)(arg0 + 6) = 0;
                *(int *)(arg1 + 4) = 0;
                *(char *)(arg1 + 0x40) = ((func_00122BB8(arg0, 2) >> 13) & 0x3F) + 0x3C;
                if (*(short *)(arg0 + 0x36) & 0x8000) {
                    *(char *)(arg1 + 0x44) = -0x10;
                }
                *(short *)(arg0 + 0x36) = 0;
            } else {
                *(unsigned char *)(arg0 + 5) = st + 1;
            }
        }
        break;
    case 2:
        if (D_0028A9A2 != 0 && D_0028A9A0 == 2) {
            *(char *)(arg0 + 4) = 3;
            if (*(int *)(arg0 + 0x24) != 0) {
                *(char *)(*(int *)(arg0 + 0x20) + 4) = 3;
                *(int *)(arg0 + 0x20) = 0;
            }
        }
        break;
    }
    func_001B5360(arg0);
    *(int *)(arg1 + 0) = anim_advance_time(arg0, *(float *)(arg1 + 0x28));
    func_00131ED0(arg0);
    func_001C68C0(arg0);
    func_001B17A0(arg0);
    (*(void (**)(int))(arg0 + 0x4C))(arg0);
}
