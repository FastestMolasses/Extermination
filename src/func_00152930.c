// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00122BB8(void);
extern int func_00153290(void);
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);

void func_00152930(char *self, char *other) {
    unsigned char st;
    int p;

    st = *(unsigned char *)(self + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(self + 6) = st + 1;
        *(int *)(other + 4) = 0;
        *(char *)(other + 0x53) = 2;
        *(float *)(other + 0x28) = 1.0f;
        anim_clip_init(self, 5, 5.0f, 0.0f);
        p = *(int *)(self + 0x20);
        if (p != 0) {
            *(char *)(p + 5) = 0;
        }
        func_001FBD50(self, 0x88F, 0, 500.0f);
        break;
    case 1:
        if (*(int *)(other + 0) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
            anim_clip_init(self, 6, 0.0f, 0.0f);
            func_001FBD50(self, 0x890, 0, 500.0f);
        }
        break;
    case 2:
        if (*(int *)(other + 0) & 0x1000) {
            *(unsigned char *)(self + 6) = st + 1;
            anim_clip_init(self, 7, 0.0f, 0.0f);
        }
        break;
    case 3:
        if (*(int *)(other + 4) == 0) {
            if (*(float *)(self + 0x3C) <= 50.0f) {
                *(int *)(other + 4) = 1;
                func_00153290();
                func_001FBD50(self, 0x886, 0, 500.0f);
                func_001FBD50(self, 0x889, 0, 500.0f);
            }
        }
        if (*(int *)(other + 0) & 0x1000) {
            float z;
            *(unsigned char *)(self + 6) = *(unsigned char *)(self + 6) + 1;
            anim_clip_init(self, 8, z = 0.0f, z);
            func_001FBD50(self, 0x890, 0, 500.0f);
        }
        break;
    case 4:
        if (*(int *)(other + 0) & 0x1000) {
            *(char *)(self + 5) = 0;
            *(char *)(self + 6) = 0;
            *(short *)(other + 0x44) = ((func_00122BB8() >> 16) & 0x1FF) + 0x64;
        }
        break;
    }
}
