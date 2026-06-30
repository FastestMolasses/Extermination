// NEARMISS func_00131740  (vram 0x00131740, 0x1F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / delay-slot-fill / branch-likely lowering permutation. Body and structure fully recovered; 9 residual instructions: (1) target duplicates 'addiu v0,v1,1' via a branch-likely beql slot fill at the case-0 entry while mwcc emits it once; (2) target fills the func_001C6160 jal...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Per-tick state handler for actor arg0 (state byte at arg0+6) against companion record arg1.
// Any state other than 0 or 1 returns immediately (without restoring the saved regs the
// other paths use -- a separate early-exit epilogue).
//   state 0 (intro): advance the state byte; clear arg1+0x63; clear arg1+0x3C; write 1.0f to
//     arg1+0x34; fire event 0x7D4 via func_001FBD50(arg0, 0x7D4, 0, 300.0f). If arg1+0x61 is
//     set AND arg0+0x2C == 4 AND (func_001C6160(arg0) - arg0.0x3C as float) >= 65.0, clear
//     arg1+0x61. Then: if arg1+0x61 is still set, play anim clip 0x27 (speed 0) and return.
//     Otherwise: if arg0+0x34 != 0 set arg0+0 = 1; if arg0+0x36 has bit 0x8000, play clip
//     0x28 and fire event 0x7D6 via func_001FBD50(arg0, 0x7D6, 0, 300.0f); else play a clip
//     chosen from D_00275390[(func_00122BB8() >> 11) % 3]. Finally clear arg0+0x36.
//   state 1 (run): if arg1+0x58 (halfword) has bit 0x1000: if arg0+0x34 != 0 latch the actor
//     (arg0+0 = 1, clear arg0+0x36, arg0+4 = 1, clear arg0+5, reset state arg0+6 = 0, clear
//     arg1+0x62, set arg1+0x6A = 0x3C) and return; else set arg0+5 = 2, reset state arg0+6 = 0.
//     If bit 0x1000 is clear and arg1+0x61 == 0, tail-call func_00133A20(arg0, arg1).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_001C6160(char *p);
extern int func_00122BB8(void);
extern void func_00133A20(char *arg0, char *arg1);
extern short D_00275390[3];

void func_00131740(char *arg0, char *arg1) {
    unsigned char state = *(unsigned char *)(arg0 + 6);
    float z;

    switch (state) {
    case 0:
        *(unsigned char *)(arg0 + 6) = state + 1;
        *(char *)(arg1 + 0x63) = 0;
        *(int *)(arg1 + 0x3C) = 0;
        *(int *)(arg1 + 0x34) = 0x3F800000;
        func_001FBD50(arg0, 0x7D4, 0, 300.0f);
        if (*(unsigned char *)(arg1 + 0x61) != 0 &&
            *(unsigned char *)(arg0 + 0x2C) == 4 &&
            ((float)func_001C6160(arg0) - *(float *)(arg0 + 0x3C)) >= 65.0f) {
            *(unsigned char *)(arg1 + 0x61) = 0;
        }
        if (*(unsigned char *)(arg1 + 0x61) != 0) {
            anim_clip_init(arg0, 0x27, z, (z = 0.0f));
            return;
        }
        if (*(short *)(arg0 + 0x34) != 0) {
            *(char *)(arg0 + 0) = 1;
        }
        if (*(short *)(arg0 + 0x36) & 0x8000) {
            anim_clip_init(arg0, 0x28, z, (z = 0.0f));
            func_001FBD50(arg0, 0x7D6, 0, 300.0f);
        } else {
            anim_clip_init(arg0, D_00275390[(func_00122BB8() >> 11) % 3], z, (z = 0.0f));
        }
        *(short *)(arg0 + 0x36) = 0;
        return;
    case 1:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            if (*(short *)(arg0 + 0x34) != 0) {
                *(char *)(arg0 + 0) = 1;
                *(short *)(arg0 + 0x36) = 0;
                *(char *)(arg0 + 4) = 1;
                *(char *)(arg0 + 5) = 0;
                *(unsigned char *)(arg0 + 6) = 0;
                *(char *)(arg1 + 0x62) = 0;
                *(char *)(arg1 + 0x6A) = 0x3C;
                return;
            }
            *(char *)(arg0 + 5) = 2;
            *(char *)(arg0 + 6) = 0;
            return;
        }
        if (*(unsigned char *)(arg1 + 0x61) == 0) {
            func_00133A20(arg0, arg1);
        }
        return;
    }
}
