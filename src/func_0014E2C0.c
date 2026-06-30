// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Two-step state machine keyed on the byte at self+6. State 0: set state +0 = 2,
// advance the +6 counter, zero the q+0x20 float field, set q+0x52 = 100, and kick
// anim_clip_init(self, 0, 0.0f, 0.0f). State 1: set q+0x52 = 100, then if the
// global mode byte D_00810803 == 2 commit the transition (state +0 = 1, clear +5
// and +6, q+0x20 = 1.0f, q+0x52 = 0, q+0x50 = 180).
extern void anim_clip_init(void *p, int n, float a, float b);
extern unsigned char D_00810803;

void func_0014E2C0(unsigned char *p, char *q)
{
  switch (p[6])
  {
    case 0:
      p[0] = 2;
      p[6] = p[6] + 1;
      *(int *)(q + 0x20) = 0;
      *(short *)(q + 0x52) = 0x64;
      anim_clip_init(p, 0, 0.0f, 0.0f);
      break;

    case 1:
      *(short *)(q + 0x52) = 0x64;
      if (D_00810803 == 2)
      {
        p[0] = 1;
        p[5] = 0;
        p[6] = 0;
        *(int *)(q + 0x20) = 0x3F800000;
        *(short *)(q + 0x52) = 0;
        *(short *)(q + 0x50) = 0xB4;
      }
      break;
  }
}
