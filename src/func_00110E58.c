// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char *func_00110AB8(int a, int b, int c, int d);
int func_00110E58(int a, int b, int c, int d)
{
  unsigned char *p = func_00110AB8(a, b, c, d);
  unsigned char e = p[0x72];
  int new_var;
  if (e != 1)
  {
    return 0;
  }
  if (p[0x71] == 2)
  {
    return 0;
  }
  switch (c)
  {
    case 1:
      if (p[0x65] == 0xF3)
    {
      return 0;
    }
      return p[0x65] >> 4;

    case 2:
      if (p[0x64] == e)
    {
      return 0;
    }
      return *((unsigned short *) ((p + 0x50) - (-(p[0x69] << 1))));

    case 3:
      if (p[0x64] == e)
    {
      return 0;
    }
      return p[0x69];

    case 4:
      if (p[0x64] == e)
    {
      return 0;
    }
      new_var = d;
      if (d == (-1))
    {
      return p[0x68];
    }
      if (new_var >= p[0x68])
    {
      return 0;
    }
      return *((unsigned short *) ((p + 0x50) - (-(new_var << 1))));

    default:
      return 0;

  }

}
