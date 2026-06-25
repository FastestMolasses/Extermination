// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// INIT of the kind-0xE tendril field: bind spike mesh, bone/matrix
// boilerplate, then tint RGB (+0x80..88) from the D_00246800 room table
// keyed by (D_00810700<<8)|D_00810701; clears the 12 record valid flags.
extern int D_0028A4E4[];
extern short D_00275BCC;
extern int D_00275450;
extern unsigned char D_00810700[];
extern unsigned char D_00810701[];
extern unsigned char D_00246800[];
extern void func_001CA5E0(unsigned char *self, int model, int mode);
extern unsigned char func_001C6150(int v);
extern int func_001AF780(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_1(unsigned char *self);

void func_00154740(unsigned char *self, unsigned char *scr, unsigned char *rec, unsigned int idx) {
    int i;
    unsigned char *cur;

    func_001CA5E0(self, D_0028A4E4[0], 0xA);
    self[0xC] = func_001C6150(*((int *) (self + 0x44)));
    if (D_00275BCC < self[0xC]) {
        self[4] = 3;
        return;
    }
    i = 0;
    cur = self;
    while (i < self[0xC]) {
        *((int *) (cur + 0x110)) = func_001AF780();
        cur += 4;
        i += 1;
    }

    self[9] = self[0xC];
    anim_bone_array_setup(self[0xC]);
    bone_init_default_1(self);
    self[4] = 1;
    self[0] = 2;
    *((int **) (self + 0x30)) = &D_00275450;
    *((int *) (self + 0x80)) = 0x3F800000;
    *((int *) (self + 0x84)) = 0x3F800000;
    *((int *) (self + 0x88)) = 0x3F800000;
    {
        unsigned int hi;
        unsigned int key;
        int t;
        hi = D_00810700[0] << 8;
        idx = 0;
        key = hi + D_00810701[0];
        rec = D_00246800;
        for (;;) {
            if ((*((unsigned int *) rec)) == key) {
                *((float *) (scr + 0x0)) = (float) ((unsigned int) rec[4]);
                *((float *) (scr + 0x4)) = (float) ((unsigned int) rec[5]);
                *((float *) (scr + 0x8)) = (float) ((unsigned int) rec[6]);
                *((float *) (scr + 0xC)) = ((float) ((unsigned int) rec[7])) / 128.0f;
                *((float *) (self + 0x80)) = (*((float *) (scr + 0x0))) / 128.0f;
                *((float *) (self + 0x84)) = (*((float *) (scr + 0x4))) / 128.0f;
                *((float *) (self + 0x88)) = (*((float *) (scr + 0x8))) / 128.0f;
                *((float *) (self + 0x8C)) = *((float *) (scr + 0xC));
                break;
            }
            t = idx + 1;
            idx = t;
            t = ((unsigned int) t) < 0x16;
            rec += 8;
            if (!t) {
                break;
            }
        }
    }
    *((int *) (self + 0x38)) = 0x3F800000;
    {
        int j;
        j = 0;
        do {
            j += 1;
            *((short *) (scr + 0x84)) = 0;
            scr += 0xA;
        } while (j < 0xC);
    }
}
