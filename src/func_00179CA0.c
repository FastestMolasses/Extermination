// CFLAGS: -O4,p -sdatathreshold 0
void func_00179CA0(float *a0, float *a1, float *a2, float t)
{
  int i = 0;
  int cond;
  float *dst = a0;
  float omt = 1.0f - t;
  do
  {
    i += 1;
    dst[0] = (omt * a1[0]) + (t * a2[0]);
    dst[1] = (omt * a1[1]) + (t * a2[1]);
    dst[2] = (omt * a1[2]) + (t * a2[2]);
    a2 += 4;
    a1 += 4;
    dst += 4;
  }
  while (cond = i < 4);
  a0[11] = (a0[7] = (a0[3] = 0.0f));
  a0[15] = 1.0f;
}
