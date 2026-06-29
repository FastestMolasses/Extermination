// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char *D_00281AD4;
void func_00118E60(void *arg)
{
  unsigned char *p = (unsigned char *) arg;
  unsigned char *base;
  unsigned int acc;
  unsigned int flag = *((unsigned short *) (p + 0x54));
  unsigned char b;
  base = D_00281AD4;
  acc = 0;
  do
  {
    int off = *((int *) (p + 0x8));
    acc <<= 7;
    b = base[off];
    *((int *) (p + 0x8)) = off + 1;
    acc |= b & 0x7F;
  }
  while (b & 0x80);
  if ((*((unsigned short *) (p + 0x54))) & 0xFFFF)
  {
    *((int *) (p + 0x20)) += acc << 12;
  }
}
