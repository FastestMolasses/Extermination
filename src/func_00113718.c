// COMPILER: eegcc
// CFLAGS: -O2

extern int func_00112E28(int a0);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern int CreateSema(int a0);
extern int D_0027AB40[];
extern int D_0027AF60[];
extern int D_00241D0C;
struct __attribute__((packed, aligned(1))) u64
{
  long v;
};
int func_00113718(struct u64 *a0, int *a1)
{
  int new_var;
  int b;
  if (func_00112E28(4) == 0)
  {
    return 0;
  }
  b = (int) (&D_0027AB40[0]);
  if (func_0010E8A8(D_0027AF60, 6, 0, 0, 0, D_0027AB40, 0x10, 0, 0) < 0)
  {
    CreateSema(D_00241D0C);
    return 0;
  }
  *a0 = *((struct u64 *) ((b + 8) | 0x20000000));
  *a1 = *((int *) ((b + 4) | 0x20000000));
  new_var = *((int *) (b | 0x20000000));
  CreateSema(D_00241D0C);
  return new_var;
}
