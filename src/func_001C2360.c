// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Sibling of func_001C22A0; identical anim/skeleton init but keyed off the
// font/model table D_0028A56C instead of D_0028A59C.
extern int D_0028A56C[];
extern short D_00275BCC;
extern int func_001C6120(int font, int code);
extern void func_001CA5E0(unsigned char *self, int model, int mode);
extern unsigned char func_001C6150(int v);
extern int func_001AF780(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_1(unsigned char *self);

int func_001C2360(unsigned char *self) {
    int i;
    unsigned char *cur;

    func_001CA5E0(self, func_001C6120(D_0028A56C[0], self[0xD]), 2);
    self[0xC] = func_001C6150(*((int *) (self + 0x44)));
    if (D_00275BCC < self[0xC]) {
        return 1;
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
    return 0;
}
