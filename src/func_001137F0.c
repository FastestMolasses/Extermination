// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00112E28(int arg);
extern int func_0010E8A8(int a0, int a1, int a2, int a3, int t0, int t1, int t2, int t3, int t4);
extern int CreateSema(int arg);
extern int D_0027AB40;
extern int D_0027AF60;
extern int D_00241D0C;

struct __attribute__((packed)) qw
{
  char b[16];
};

int func_001137F0(struct qw *out, int *psecond)
{
  int base;
  unsigned int new_var;
  if (func_00112E28(5) == 0)
  {
    return 0;
  }
  base = (int) (&D_0027AB40);
  if (func_0010E8A8((int) (&D_0027AF60), 0x1A, 0, 0, 0, base, 0x18, 0, 0) < 0)
  {
    CreateSema(D_00241D0C);
    return 0;
  }
  *out = *((struct qw *) ((base + 8) | 0x20000000));
  *psecond = *((int *) ((base + 4) | 0x20000000));
  new_var = *((int *) (base | 0x20000000));
  CreateSema(D_00241D0C);
  return new_var;
}
