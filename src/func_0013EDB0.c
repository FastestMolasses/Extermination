// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.96% NEARMISS).
// Fixes read from the original instructions:
// - every switch case ends in `break` (the original duplicates the case-0
//   increment and the threshold constants at the branch targets), and the
//   pitch dispatch is an if / else-if / else chain, not early returns;
// - the pitch step is `y += 10.0f` on the scratch vector element (a compound
//   assignment), with the scratch Y words spelled as elements of the relocated
//   D_700038A0 / D_700038B0 vectors so their loads can be scheduled ahead of
//   the neighbouring literal stores;
// - case 2 raises the origin's Y before writing the target's Z/W words
//   (0x700038B8/0x700038BC as D_700038B0[2]/[3]).

// Per-state enemy behavior handler. arg0 is the actor, arg1 the paired
// control record. Dispatches on the state byte at arg0+6 (cases 0/1/2).
//   case 0: bump state, zero arg1+0x20 (mode), set arg1+0x8D=1, arg1+0x81=0,
//     arg1+0x40=1.0f, arg1+0x48/0x50/0x4C=0, then anim_clip_init(self,
//     0xC, 5.0f, 0.0f).
//   case 1: aim yaw at arg0+0x44 via func_001B1240(self+0xB0, target xy),
//     steer arg0+0xC4 toward it via func_001B12B0 (rate 0.0698132f, 4 degrees in radians).
//     If arg1+0x30 bit 0x1000 set, bump state and compute a pitch value
//     staged through the scratch globals at 0x700038Ax/0x70003Ax (uses
//     func_00102948/func_0011DF78/func_001B15D0/func_0011E620), clamps it to
//     >=0, stores it to arg1+0x54, and dispatches anim_clip_init(self, 0xD/
//     0xE/0xF, 0, 0) based on threshold comparisons against -0.5235988f /
//     -1.0471976f.
//   case 2: once (arg1+0x20==0 && arg0+0x3C<=16.0f), stage a quat at the
//     0x700038Ax scratch, run func_001B2B10/func_001028B8, and if
//     func_0019A6F0(...) succeeds, build a rotated aim vector via
//     func_00102948/func_001029C0/func_00102B08/func_00102BB0/func_001026A0
//     and dispatch func_001EFFD0 (variant on arg0+0xD bit 0x80) then
//     func_001FBD50(self, 0x809, 0, 300.0f). Either way, if arg1+0x30 bit
//     0x1000 set, reset the actor (arg0+5=4, arg0+6=0, arg1+0x8D=0).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B12B0(float goal, float cur, float rate);
extern void func_00102948(void *dst, void *src);
extern float func_0011DF78(float a);
extern float func_001B15D0(void *a, void *b);
extern float func_0011E620(float y, float x);
extern void func_001B2B10(char *p, float *a, float *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019A6F0(char *arg0, void *arg1, void *arg2, int arg3, int arg4);
extern void func_001029C0(void *m);
extern void func_00102B08(void *a0, void *a1, float b);
extern void func_00102BB0(void *a0, void *a1, float b);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001EFFD0(int a, void *b, void *c, int d, float e);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int D_00275B40;
extern float D_00810360[8];
extern float D_00810368[8];
extern float D_700036A0[8];
extern float D_700038A0[8];
extern float D_700038B0[8];

void func_0013EDB0(char *arg0, char *arg1) {
  unsigned char st;
  float ang;

  st = *(unsigned char *)(arg0 + 6);
  switch (st) {
  case 0:
    *(unsigned char *)(arg0 + 6) = st + 1;
    *(int *)(arg1 + 0x20) = 0;
    *(char *)(arg1 + 0x8D) = 1;
    *(char *)(arg1 + 0x81) = 0;
    *(float *)(arg1 + 0x40) = 1.0f;
    *(int *)(arg1 + 0x48) = 0;
    *(int *)(arg1 + 0x50) = 0;
    *(int *)(arg1 + 0x4C) = 0;
    anim_clip_init(arg0, 0xC, 5.0f, 0.0f);
    break;
  case 1:
    *(float *)(arg1 + 0x44) = func_001B1240(arg0 + 0xB0, D_00810360[0], D_00810368[0]);
    *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x44), *(float *)(arg0 + 0xC4), 0.06981316953897476f);
    if (*(int *)(arg1 + 0x30) & 0x1000) {
      *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
      func_00102948(D_700038A0, (void *)(*(int *)(D_00275B40 + 0x3C) + 0xC0));
      func_00102948(D_700038B0, D_00810360);
      D_700038B0[1] += 10.0f;
      *(float *)0x70003A20 = func_0011DF78(D_700038A0[1] - D_700038B0[1]);
      D_700038B0[1] = 0.0f;
      D_700038A0[1] = 0.0f;
      *(float *)0x70003A24 = func_001B15D0(D_700038A0, D_700038B0);
      ang = -1.0f * func_0011E620(*(float *)0x70003A20, *(float *)0x70003A24);
      *(float *)0x70003A20 = ang;
      if (!(ang <= 0.0f)) {
        *(float *)0x70003A20 = 0.0f;
      }
      *(float *)(arg1 + 0x54) = *(float *)0x70003A20;
      if (!(*(float *)0x70003A20 < -0.5235988f)) {
        anim_clip_init(arg0, 0xD, 0.0f, 0.0f);
      } else if (!(*(float *)0x70003A20 < -1.0471976f)) {
        anim_clip_init(arg0, 0xE, 0.0f, 0.0f);
      } else {
        anim_clip_init(arg0, 0xF, 0.0f, 0.0f);
      }
    }
    break;
  case 2:
    if (*(int *)(arg1 + 0x20) == 0 && *(float *)(arg0 + 0x3C) <= 16.0f) {
      *(int *)(arg1 + 0x20) = 1;
      func_00102948(D_700038A0, arg0 + 0xB0);
      *(int *)0x700038B0 = 0;
      *(float *)0x700038B4 = 0.0f;
      D_700038A0[1] += 10.0f;
      D_700038B0[2] = 12.0f;
      D_700038B0[3] = 1.0f;
      func_001B2B10(arg0, D_700038B0, D_700038B0);
      func_001028B8(D_700038B0, D_700038B0, D_700038A0);
      if (func_0019A6F0(arg0, D_700038A0, D_700038B0, 7, 0x40) == 0) {
        func_00102948(D_700038A0, (void *)(*(int *)(D_00275B40 + 0x3C) + 0xC0));
        func_001029C0(D_700036A0);
        func_00102B08(D_700036A0, D_700036A0, *(float *)(arg1 + 0x54));
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = 0.0f;
        *(float *)0x700038B8 = 1.0f;
        *(float *)0x700038BC = 1.0f;
        func_001026A0(D_700038B0, D_700036A0, D_700038B0);
        if (*(unsigned char *)(arg0 + 0xD) & 0x80) {
          func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0x11, 0.7f);
        } else {
          func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0x10, 0.7f);
        }
        func_001FBD50(arg0, 0x809, 0, 300.0f);
      }
    }
    if (*(int *)(arg1 + 0x30) & 0x1000) {
      *(char *)(arg0 + 5) = 4;
      *(unsigned char *)(arg0 + 6) = 0;
      *(char *)(arg1 + 0x8D) = 0;
    }
    break;
  }
}
