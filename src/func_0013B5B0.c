// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-state ambient tick for actor `arg0` with anim-context `arg1`: state 0 arms the
// idle timer (-1.8f / 0.05f at ctx+0x44/+0x48), pings func_001FBD50(0x838, 300.0f) and
// kicks anim clip 0xF or 0x10 from an RNG bit, then falls into state 1, which clamps the
// timer at 0, and once it goes negative stages the scratchpad vector at 0x700038A0 with
// (0, 0, -3.0f, 1.0f) and runs the func_001B2B10 / func_001028B8 / func_0019AD00 trio;
// finally, when ctx+0x70 has bit 0x1000 set, it resets the actor state bytes, clears the
// timer, seeds ctx+0x50 = -1.5533f, clears ctx+0x20/+0x22, and (if arg0+0x36 & 0x5000)
// sets ctx+0x34 = 10 before clearing arg0+0x36.
//
// Two load-bearing source shapes here:
//  * idiom-24: `zi = 0; z = (float)zi;` before the clip-0xF call is what makes mwcc emit
//    `mtc1 zero,$f13` ahead of `mtc1 $v0,$f12`; a plain 0.0f literal reverses them.
//  * case 1 must END WITHOUT an explicit `return;` (control falls out of the switch).
//    The explicit `return` turns the switch tail into a region mwcc's post-RA scheduler
//    will speculate into: it then fills the bc1f / two beqz delay slots and deletes the
//    two dead const re-materializations, costing 5 instructions (95.80% vs 100.0%).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_00122BB8(void);
extern void func_001B2B10(char *p, float *a, float *b);
extern void func_001028B8(float *a, float *b, char *c);
extern void func_0019AD00(char *p, float *a, int b);
extern float D_700038A0;

void func_0013B5B0(char *arg0, char *arg1)
{
  unsigned char st;
  int zi;
  float z;

  st = *(unsigned char *)(arg0 + 6);
  switch (st) {
  case 0:
    *(unsigned char *)(arg0 + 6) = st + 1;
    *(float *)(arg1 + 0x44) = -1.8f;
    *(int *)(arg1 + 0x48) = 0x3D4CCCCD;
    func_001FBD50(arg0, 0x838, 0, 300.0f);
    if ((func_00122BB8() >> 0x11) & 3) {
      zi = 0;
      z = (float)zi;
      anim_clip_init(arg0, 0xF, 1.0f, z);
    } else {
      anim_clip_init(arg0, 0x10, 1.0f, 0.0f);
    }
  case 1:
    if (!(*(float *)(arg1 + 0x44) <= 0.0f)) {
      *(int *)(arg1 + 0x48) = 0;
      *(float *)(arg1 + 0x44) = 0.0f;
    }
    if (*(float *)(arg1 + 0x44) < 0.0f) {
      *(int *)0x700038A0 = 0;
      *(int *)0x700038A4 = 0;
      *(int *)0x700038A8 = 0xC0400000;
      *(int *)0x700038AC = 0x3F800000;
      func_001B2B10(arg0, &D_700038A0, &D_700038A0);
      func_001028B8(&D_700038A0, &D_700038A0, arg0 + 0xB0);
      func_0019AD00(arg0, &D_700038A0, 0x80000006);
    }
    if (*(int *)(arg1 + 0x70) & 0x1000) {
      *(char *)(arg0 + 0) = 1;
      *(char *)(arg0 + 4) = 1;
      *(char *)(arg0 + 5) = 2;
      *(unsigned char *)(arg0 + 6) = 0;
      *(int *)(arg1 + 0x48) = 0;
      *(float *)(arg1 + 0x44) = 0.0f;
      *(int *)(arg1 + 0x50) = 0xBFC6D3F2;
      *(short *)(arg1 + 0x22) = 0;
      *(short *)(arg1 + 0x20) = 0;
      if (*(short *)(arg0 + 0x36) & 0x5000) {
        *(short *)(arg1 + 0x34) = 0xA;
      }
      *(short *)(arg0 + 0x36) = 0;
    }
  }
}
