// NEARMISS func_0013F240  (vram 0x0013F240, 0x528 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring at the D_700038A4+=10.0f add (target: add.s f0,f1,f0 field-first; mwcc233: swapped), plus a D_0081070A byte-load scheduling-order artifact in case 2 relative to the two float subtracts. Both are compiler-artifact classes documented in func_0021F330's NEARMISS; no source resha...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// arg0 = self, arg1 = owner-state block. Per-state driver on state byte arg0+6
// (states 0/1/2/3).
// state 0: bump state, init owner fields (target position 0x20, flags 0x8D/0x81,
//  ease scalar 0x40=1.0f, timer 0x48=0, coloring byte 0x86 masked to bit0, counters
//  0x50/0x4C zeroed), kick clip 0x11 (a startup/aim anim).
// state 1: steer arg0+0xC4 (a facing/aim angle) toward the target direction via
//  func_001B1240 + func_001B12B0 easing (rate 0.0698 rad ~= 4 deg/frame). On the
//  owner's 0x1000 input bit: bump state, arm the recovery timer 0x48=1.5f and ease-
//  rate 0x4C, kick clip 0x12 (release swing) + hit-sound 0x807. If coloring byte
//  0x86==1, play a muzzle/impact vfx at bone arg0+0xB0 (scratch vec4 math via
//  func_001B2B10/func_001028B8/func_001EFD90/func_001E8B90). Always then build a second
//  scratch vec4 (a forward probe point) and call func_001B3390 (an angle/reachability
//  test with a pi/4 threshold); on success, clear the ease-rate counter 0x4C.
// state 2: mark owner+0x85=1 and clear arg0+0x5E (aim-lock flag) each tick. On first
//  entry (owner+0x20==0) while close enough (arg0+0x3C >= 16.0f) and unobstructed
//  (func_0021BE40==0) and target-visible (func_001A7B80!=0): latch owner+0x20=1, set
//  the "engaged" flag D_008102B0|=2, snapshot the aim delta into D_00810320/0x328 (dx/dz)
//  and reset D_00810324 (dy), pick a base speed D_008104D4 (difficulty-scaled via
//  D_0081070A), normalize via func_00102760, and roll a random taunt via func_001B55E0.
//  If the recovery timer 0x48 has gone negative and coloring byte 0x86==1, replay the
//  muzzle vfx once more (byte |= 2 to latch "played"). On owner+0x80 bit 2 (finished):
//  bump state, arm 0x81=1, clear 0x4C, kick clip 0x13 (recovery) + sound 0x7F9.
// state 3: on the owner's 0x1000 bit, reset to idle (arg0+5=4, state=0, owner flags
//  0x8D/0x81=0), spin arg0+0xC4 by +pi via func_001B1470, kick clip 2 (idle).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102760(float *a, float *b);
extern void func_001028B8(char *a, char *b, char *c);
extern void func_00102948(char *a, char *b);
extern int func_00122BB8(void);
extern int func_001A7B80(char *a);
extern float func_001B1240(char *a, unsigned char b, float c, float d);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(unsigned char a, float b);
extern void func_001B2B10(char *a, char *b, char *c);
extern int func_001B3390(char *a, char *b, char *c, float d);
extern void func_001B55E0(char *a, int b);
extern void func_001E8B90(char *a, float b);
extern void func_001EFD90(int a, char *b, char *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(unsigned char *a, char *b);

extern unsigned char D_008102B0;
extern float D_00810320;
extern int D_00810324;
extern float D_00810328;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern char D_700038A0[];
extern char D_700038B0[];

void func_0013F240(char *arg0, char *arg1) {
    unsigned char st;
    signed char c86;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x20) = 0;
        *(signed char *)(arg1 + 0x8D) = 1;
        *(signed char *)(arg1 + 0x81) = -0x80;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(float *)(arg1 + 0x48) = 0.0f;
        *(signed char *)(arg1 + 0x86) = *(signed char *)(arg1 + 0x86) & 1;
        *(int *)(arg1 + 0x50) = 0;
        *(int *)(arg1 + 0x4C) = 0;
        anim_clip_init(arg0, 0x11, 5.0f, 0.0f);
        break;
    case 1:
        *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, st, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.06981316954f);
        if (!(*(int *)(arg1 + 0x30) & 0x1000)) {
            break;
        }
        *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
        *(float *)(arg1 + 0x48) = 1.5f;
        *(int *)(arg1 + 0x4C) = 0x3F59999A;
        anim_clip_init(arg0, 0x12, 0.0f, 0.0f);
        func_001FBD50(arg0, 0x807, 0, 300.0f);
        if (*(signed char *)(arg1 + 0x86) == 1) {
            *(int *)0x700038A0 = 0;
            *(float *)0x700038A4 = 0.0f;
            *(int *)0x700038A8 = 0x40200000;
            *(int *)0x700038AC = 0x3F800000;
            func_001B2B10(arg0, D_700038A0, D_700038A0);
            func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
            *(float *)0x700038A4 = *(float *)(arg1 + 0x60);
            func_001EFD90(0x80000016, D_700038A0, arg0 + 0xC0);
            func_001E8B90(D_700038A0, 5.0f);
        }
        func_00102948(D_700038A0, arg0 + 0xB0);
        *(int *)0x700038B0 = 0;
        *(int *)0x700038B4 = 0;
        *(float *)0x700038A4 = *(float *)0x700038A4 + 10.0f;
        *(int *)0x700038B8 = 0x41100000;
        *(int *)0x700038BC = 0x3F800000;
        func_001B2B10(arg0, D_700038B0, D_700038B0);
        func_001028B8(D_700038B0, D_700038B0, D_700038A0);
        if (func_001B3390(arg0, D_700038A0, D_700038B0, 0.785398185f) != 0) {
            *(int *)(arg1 + 0x4C) = 0;
        }
        break;
    case 2:
        *(signed char *)(arg1 + 0x85) = 1;
        *(signed char *)(arg0 + 0x5E) = 0;
        if (*(int *)(arg1 + 0x20) == 0 && !(*(float *)(arg0 + 0x3C) < 16.0f) &&
            func_0021BE40(&D_008102B0, arg0) == 0 && func_001A7B80(arg0) != 0) {
            *(int *)(arg1 + 0x20) = 1;
            D_008102B0 |= 2;
            D_00810320 = D_00810360 - *(float *)(arg0 + 0xB0);
            D_00810324 = 0;
            D_00810328 = D_00810368 - *(float *)(arg0 + 0xB8);
            if (D_0081070A != 0) {
                D_008104D4 = 0x41C80000;
            } else {
                D_008104D4 = 0x41A00000;
            }
            func_00102760(&D_00810320, &D_00810320);
            func_001B55E0(arg0, (signed char)((func_00122BB8() >> 7) & 1));
        }
        if (*(float *)(arg1 + 0x48) < 0.0f) {
            c86 = *(signed char *)(arg1 + 0x86);
            if (c86 == 1) {
                *(signed char *)(arg1 + 0x86) = c86 | 2;
                *(int *)0x700038A0 = 0;
                *(float *)0x700038A4 = 0.0f;
                *(int *)0x700038A8 = 0x40A00000;
                *(int *)0x700038AC = 0x3F800000;
                func_001B2B10(arg0, D_700038A0, D_700038A0);
                func_001028B8(D_700038A0, arg0 + 0xB0, D_700038A0);
                *(float *)0x700038A4 = *(float *)(arg1 + 0x60);
                func_001EFD90(0x80000016, D_700038A0, arg0 + 0xC0);
                func_001E8B90(D_700038A0, 5.0f);
            }
        }
        if (*(signed char *)(arg1 + 0x80) & 2) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(signed char *)(arg1 + 0x81) = 1;
            *(int *)(arg1 + 0x4C) = 0;
            anim_clip_init(arg0, 0x13, 0.0f, 0.0f);
            func_001FBD50(arg0, 0x7F9, 0, 300.0f);
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0x30) & 0x1000) {
            *(signed char *)(arg0 + 5) = 4;
            *(unsigned char *)(arg0 + 6) = 0;
            *(signed char *)(arg1 + 0x8D) = 0;
            *(signed char *)(arg1 + 0x81) = 0;
            *(float *)(arg0 + 0xC4) = func_001B1470(st, 3.14159274101f + *(float *)(arg0 + 0xC4));
            anim_clip_init(arg0, 2, 0.0f, 0.0f);
        }
        break;
    }
}
