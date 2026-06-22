// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Initializes a 16x16 table of 0x18-byte records at D_0081E0F0 (0x180 per row).
// Each record: [0]=fi=256*j/16, [4]=fo=256*i/16, [8]=0, [0xC]=0. The fo computation
// is written inside the inner loop and 16.0f is hoisted to a local to reproduce CW's
// FP-divide scheduling (the 991202 build software-pipelines the divides differently).
extern unsigned char D_0081E0F0[];

void func_001E1010(void) {
    char *new_var;
    int j;
    int i;
    char *e;
    float new_var2;
    char *row;
    float fo;
    float fi;

    i = 0;
    row = (char *)D_0081E0F0;
    do {
        j = 0;
        e = row;
        do {
            fi = (256.0f * ((float)j)) / 16.0f;
            *((float *)(e + 0x0)) = fi;
            *((float *)(e + 0x4)) = (256.0f * ((float)i)) / (new_var2 = 16.0f);
            *((int *)(e + 0x8)) = 0;
            new_var = e + 0xC;
            *((int *)new_var) = 0;
            j++;
            e += 0x18;
        } while (j < 16);
        i++;
        row += 0x180;
    } while (i < 16);
}
