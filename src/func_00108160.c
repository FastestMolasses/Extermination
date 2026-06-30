// COMPILER: eegcc
// CFLAGS: -O2

extern void func_001063B8(int arg);
extern int func_00109B20(int chan, void *buf);
extern int D_002412F4;

void func_00108160(int src, int qwc)
{
  int local[8];
  while ((*((volatile int *) 0x10002010)) < 0)
  {
    ;
  }

  *((volatile int *) 0x1000B010) = src & 0x0FFFFFFF;
  *((volatile int *) 0x1000B020) = qwc << 6;
  *((volatile int *) 0x1000B000) = 0x100;
  func_001063B8(0x70000000 | qwc);
  {
    int chan = D_002412F4;
    local[0] = 4;
    func_00109B20(chan, local);
  }
  while (((*((volatile unsigned int *) 0x1000B000)) >> 8) & 1)
  {
    ;
  }

  while ((*((volatile int *) 0x10002010)) < 0)
  {
    ;
  }

}
