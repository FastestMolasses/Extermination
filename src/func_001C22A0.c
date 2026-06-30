// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Anim/skeleton init for a kind-variant entity (font/model table D_0028A59C).
// Binds model via func_001C6120(font, self[0xD]) + func_001CA5E0(self, model, 2),
// reads bone count into self[0xC] from func_001C6150(self[0x44]); if it exceeds
// the global cap D_00275BCC returns 1, else fills the per-bone pointer array at
// self+0x110 from func_001AF780(), records the count in self[9], runs
// anim_bone_array_setup(count) + bone_init_default_1(self), returns 0.
extern int D_0028A59C[];
extern short D_00275BCC;
extern int func_001C6120(int font, int code);
extern void func_001CA5E0(unsigned char *self, int model, int mode);
extern unsigned char func_001C6150(int v);
extern int func_001AF780(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_1(unsigned char *self);

int func_001C22A0(unsigned char *self) {
    int i;
    unsigned char *cur;

    func_001CA5E0(self, func_001C6120(D_0028A59C[0], self[0xD]), 2);
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
