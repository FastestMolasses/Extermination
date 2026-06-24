// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Animation/state-machine step keyed on arg0[6] (0/1/2). Case 0 inits anim
// clip + float pose fields on arg1 and runs a callback chain; case 1 advances
// the clip when a flag bit is set; case 2 resets state on the same flag bit.
void anim_clip_init(void *a, int b, float c, float d);
void func_001FBD50(void *a, int b, int c, float d);
float func_0011E620(float a, float b);
float func_001B1470(float a);
void func_0021C040(void *a, void *b);
extern unsigned char D_008102B0[8];
extern unsigned char D_008106BD;

void func_001408D0(unsigned char *arg0, unsigned char *arg1) {
    unsigned char st;

    st = arg0[6];
    switch (st) {
    case 0:
        arg0[6] = st + 1;
        arg1[0x81] = 0;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(int *)(arg1 + 0x48) = 0x3FC00000;
        *(int *)(arg1 + 0x4C) = 0xBF800000;
        *(int *)(arg1 + 0x50) = 0;
        anim_clip_init(arg0, 0x22, 0.0f, 0.0f);
        func_001FBD50(arg0, 0x80E, 0, 300.0f);
        *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + func_0011E620(*(float *)(arg0 + 0x70), *(float *)(arg0 + 0x78)));
        if (*(signed char *)(arg1 + 0x89) != 0) {
            arg1[0x89] = 0;
            func_0021C040(D_008102B0, arg0);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        break;
    case 1:
        if (*(char *)(arg1 + 0x80) & 2) {
            *(int *)(arg1 + 0x4C) = 0;
        }
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            if (*(short *)(arg0 + 0x34) != 0) {
                arg0[6] = arg0[6] + 1;
                anim_clip_init(arg0, 0x24, 1.0f, 0.0f);
            } else {
                arg0[5] = 2;
                arg0[6] = 0;
                arg1[0x8C] = 1;
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            arg0[4] = 1;
            arg0[5] = 8;
            arg0[6] = 0;
            arg0[0] = 1;
            *(short *)(arg0 + 0x36) = 0;
            arg1[0x88] = 0;
            arg1[0x8D] = 0;
            *(short *)(arg1 + 0x70) = 0xF0;
        }
        break;
    }
}
