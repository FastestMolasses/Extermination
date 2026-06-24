// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Loops i in [0,8): for each i < arg0[0xA] writes 0x7F into a 1-byte scratch slot
// (which was pre-seeded with gp-rel halfword D_00275868) and calls
// func_001CC1E0(1, 0x7C2+0x10*i, 0x7E8, 0xA, 0x14, &scratch, 0) then
// func_00207D00(1, 3). Every iteration calls func_00207E40(1,
// float_to_int(16.0 * (0x7C0+0x10*i)), 0x7F00, 0x10, 0x10, 0x80808080,
// *(s64 *)(arg1 + 0x88)).
int float_to_int(float);
int func_001CC1E0(int, int, int, int, int, char *, int);
int func_00207D00(int, int);
int func_00207E40(int, int, int, int, int, int, long long);
extern short D_00275868;

void func_00207BB0(unsigned char *arg0, unsigned char *arg1)
{
  char spbuf[4];
  int i;
  short *new_var;
  int v1;
  int v2;
  v1 = 0x7C2;
  v2 = 0x7C0;
  *(new_var = (short *) spbuf) = D_00275868;
  for (i = 0; i < 8; i++)
  {
    if (i < arg0[0xA])
    {
      *spbuf = 0x7F;
      func_001CC1E0(1, v1, 0x7E8, 0xA, 0x14, spbuf, 0);
      func_00207D00(1, 3);
    }
    func_00207E40(1, float_to_int(16.0f * ((float) v2)), 0x7F00, 0x10, 0x10, (int) 0x80808080, *((long long *) (arg1 + 0x88)));
    v1 += 0x10;
    v2 += 0x10;
  }

}
