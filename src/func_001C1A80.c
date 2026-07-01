// NEARMISS func_001C1A80  (vram 0x001C1A80, 0x27C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two fused compiler artifacts, both confirmed by full instruction-level diff (every branch/call/store address matches): (1) constant-multiply register-coloring — the (rand>>16)*15-chain idiom (used for both the *0xF0 and *0xB4 randomized-timer reload) always gets a second sll/sra step; target colo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// SEMANTICS: Actor state-machine init/update, keyed on self[4] (0..3).
// State 0 (spawn): initializes the bone/segment array (func_001D0C80) with
// the shared skeleton table D_0028A508, seeds the animation slot at
// self+0x90 (func_001D0D40) from D_0024F8F0 with type 0x28 and flag 1, runs
// the default bone pose (bone_init_default_1), installs the shared bone-
// matrix default table &D_00275660 at self+0x30 and self+0x1F0+0x18 (tail),
// advances the state to 1, clears the "impact" flag self[0xA], rolls a
// random motion-timer value into self+0x28 (240*rand>>15 + 240, i.e. a
// 240..479 frame range via func_00122BB8), and kicks off a blend-weight ramp
// via func_001D0D60(handle, 0x30000000, 1.0 + 19.0*(2^-31 * rand)) -- a
// randomized initial weight in roughly [1.0, 20.0).
// State 1 (active tick): if the per-object gate func_001B17A0(self) passes,
// decrements the self+0x28 motion-timer; on reaching 0, plays a proximity
// sound (func_001FBD50, sound 0x441, radius 300.0) and re-rolls the timer
// (180*rand>>15 + 300, i.e. 300..479). Then checks proximity to the player
// (func_001BE5F0 against the shared actor D_008102B0 and the tail block):
// on contact, sets a debug-notify flag (func_00187EC0(8,0)) and toggles the
// "impact" flag self[0xA] -- first contact copies the player's 0xA0 vector
// slot onto self (func_00102948); a repeat contact instead recomputes a
// scaled reflection (func_001028D0/func_00102900 by -0.7, func_00183010)
// through the shared scratch D_700038A0 before copying. No contact clears
// self[0xA]. Either way, rebuilds the world matrix (func_001C6380), snaps
// the blend weight back to 1.0 (func_001D0D60(handle, 1.0)), then calls the
// object's virtual update function pointer at self+0x4C.
// States 2/3 (teardown): releases the bone-array handle (func_001AF890) and
// frees the actor back to the pool (func_001AFC10).
extern void bone_init_default_1(unsigned char *self);
extern void func_001028D0(void *out, void *a, void *b);
extern void func_00102900(void *out, void *a, float s);
extern void func_00102948(void *dst, void *src);
extern int func_00122BB8(void);
extern void func_00183010(void *a, void *b);
extern void func_00187EC0(char a0, char a1);
extern void func_001AF890(unsigned char *handle);
extern void func_001AFC10(unsigned char *handle);
extern int func_001B17A0(unsigned char *self);
extern int func_001BE5F0(void *a, unsigned char *b, unsigned char *c);
extern void func_001C6380(unsigned char *self);
extern void func_001D0C80(int a0, int a1);
extern void func_001D0D40(int *a0, int a1, int a2, int a3);
extern void func_001D0D60(int a0, int a1, float f12);
extern int func_001FBD50(unsigned char *a0, int a1, int a2, float f12);
extern int D_0028A508[];
extern unsigned char D_0024F8F0[];
extern int D_00275660;
extern unsigned char D_008102B0[];
extern unsigned char D_700038A0[];

void func_001C1A80(unsigned char *self) {
    unsigned char *tail = self + 0x1F0;
    unsigned char *g = D_008102B0;

    switch (self[4]) {
    case 0:
        func_001D0C80((int)self, D_0028A508[0]);
        func_001D0D40((int *)self, (int)D_0024F8F0, 0x28, 1);
        bone_init_default_1(self);
        *(int *)(self + 0x30) = (int)&D_00275660;
        *(int *)(tail + 0x18) = (int)&D_00275660;
        self[4] = 1;
        self[0xA] = 0;
        *(short *)(self + 0x28) = (short)((((func_00122BB8() >> 16) * 0xF0) >> 15) + 0xF0);
        func_001D0D60(*(int *)(self + 0x90), 0x30000000,
                      1.0f + 19.0f * (4.656613e-10f * (float)func_00122BB8()));
        break;
    case 1:
        if (func_001B17A0(self) != 0) {
            short cd = *(short *)(self + 0x28);
            if (cd == 0) {
                func_001FBD50(self, 0x441, 0, 300.0f);
                cd = (short)((((func_00122BB8() >> 16) * 0xB4) >> 15) + 0x12C);
                *(short *)(self + 0x28) = cd;
            } else {
                cd = cd - 1;
                *(short *)(self + 0x28) = cd;
            }
            if (func_001BE5F0(g, self, tail) != 0) {
                func_00187EC0(8, 0);
                if (self[0xA] == 0) {
                    self[0xA] = 1;
                    func_00102948(self + 0xA0, g + 0xA0);
                } else {
                    self[0xA] = 1;
                    func_001028D0(D_700038A0, g + 0xA0, self + 0xA0);
                    func_00102900(D_700038A0, D_700038A0, -0.6999999881f);
                    func_00183010(g, D_700038A0);
                    func_00102948(self + 0xA0, g + 0xA0);
                }
            } else {
                self[0xA] = 0;
            }
        } else {
            self[0xA] = 0;
        }
        func_001C6380(self);
        func_001D0D60(*(int *)(self + 0x90), (int)self, 1.0f);
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001AF890(*(unsigned char **)(self + 0x90));
        func_001AFC10(self);
        break;
    }
}
