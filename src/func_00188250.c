// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Actor state-machine step keyed on the byte at +5: state 0 (if +0x2E flag bit
// 0x1000 set, advance to 1) falls through to state 1 (if gp-rel global
// (&D_00275B00)[2] is nonzero, set state 2 and kick anim_clip_init(self, 0xD,
// 0.0, 0.0)); state 2 sets the +4 byte to 3 when the same flag bit is set. Then
// unconditionally copies the 4-qword block at D_00810380 into self+0xD0 and runs
// func_001C9940(self+0x110, self[0xC], self+0xD0); finally if D_008102B4 == 2 set
// the +4 byte to 3.
extern void anim_clip_init(void *p, int n, float a, float b);
extern void copy_qw4(char *dst, char *src);
extern void func_001C9940(char *dst, int type, char *src);
extern int D_00275B00;
extern unsigned char D_008102B4[256];
extern char D_00810380[];

void func_00188250(char *p)
{
  float new_var;
  unsigned char state;
  state = p[5];
  switch (state)
  {
    case 0:
      if ((*((unsigned short *) (p + 0x2E))) & 0x1000)
    {
      p[5] = state + 1;
    }

    case 1:
      if ((&D_00275B00)[2] != 0)
    {
      p[5] = 2;
      anim_clip_init(p, 0xD, new_var = 0.0f, new_var);
    }
      break;

    case 2:
      if ((*((unsigned short *) (p + 0x2E))) & 0x1000)
    {
      p[4] = 3;
    }
      break;

  }

  copy_qw4(p + 0xD0, D_00810380);
  func_001C9940(p + 0x110, (unsigned char) p[0xC], p + 0xD0);
  if (D_008102B4[0] == 2)
  {
    p[4] = 3;
  }
}
