// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D68;
struct s_b0c0
{
  char pad[0x24];
  int f24;
};
extern struct s_b0c0 D_0027B0C0;
extern char D_0027B184[];
extern int D_0027C680;
extern unsigned char func_00123418(char *, int, int);
extern int func_0010E8A8(struct s_b0c0 *, int, int, char *, int, int *, int, int, int);
int func_00114DA8(int a0, int a1, int a2)
{
  char *p;
  char *q;
  int r;
  if (D_00241D68 != 0)
  {
    return D_00241D68;
  }
  if (D_0027B0C0.f24 == 0)
  {
    return -0x64;
  }
  p = D_0027B184;
  func_00123418(p, a2, 0x3FF);
  q = p - 0x14;
  *((int *) (p - 0x14)) = a0;
  *((int *) (q + 4)) = a1;
  *((char *) (q + 0x413)) = 0;
  *((int *) (q + 8)) = 0;
  r = func_0010E8A8(&D_0027B0C0, 0xF, 1, q, 0x414, &D_0027C680, 4, 0, 0);
  if (r == 0)
  {
    D_00241D68 = 0xF;
  }
  return r;
}
