// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.83% NEARMISS).
// Fixes read from the original instructions (spelling taken from the byte-matched
// twin func_001C22A0):
//  - func_001C6120's second parameter is int, not unsigned char. Together with
//    the items below this removes the old "prologue schedule" residual (the +0xD
//    byte is now read through the parameter register before self is saved).
//  - anim_bone_array_setup takes an int count; D_0028A59C is an unsized int array.
//  - the bone-count reads are plain self[0xC] byte reads (no separate count local).
//
// Animation/skeleton init for object `self`. Resolves an animation handle via
// func_001C6120(D_0028A59C, self+0xD) and binds it with func_001CA6E0; sets the
// bone count self+0xC = func_001C6150(*(self+0x44)). If the count exceeds the
// global cap D_00275BCC, flags self+4 = 3 (error/over-cap) and returns 1.
// Otherwise allocates per-bone state: for each of the self+0xC bones, stores
// func_001AF780() into the self+0x110 array (4-byte stride), records the final
// count at self+9, calls anim_bone_array_setup(self+0xC) and returns 0.
extern int D_0028A59C[];
extern short D_00275BCC;
extern int func_001C6120(int font, int code);
extern void func_001CA6E0(unsigned char *self, int model);
extern unsigned char func_001C6150(int v);
extern int func_001AF780(void);
extern void anim_bone_array_setup(int n);

int func_001B0EA0(unsigned char *self) {
    int i;
    unsigned char *cur;

    func_001CA6E0(self, func_001C6120(D_0028A59C[0], self[0xD]));
    self[0xC] = func_001C6150(*((int *) (self + 0x44)));
    if (D_00275BCC < self[0xC]) {
        self[4] = 3;
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
    return 0;
}
