// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Allocates a 0xB0-byte display/effect record from the bump cursor at
// D_00275670[arg0][4]: stamps the header (byte +3=0x10, word +4=0, half +0=0xA),
// advances the cursor by 0xB0, writes a GIFtag word (+0x1C=0x50000009), a
// doubleword built from (arg3|4)<<0x2F or'd with 0x80004000_00008001 (+0x20) and
// 0x41414141 (+0x28), then copies eight 0x10-byte blocks (via func_00102948) from
// the two 0x40-byte payloads arg2/arg1 interleaved into +0x20..+0x90. Returns the
// record body pointer (cursor +0x10).
typedef unsigned __int128 u128;
extern char **D_00275670;
extern void func_00102948(void *dst, void *src);
char *func_001D6580(int arg0, char *arg1, char *arg2, int arg3) {
  char *new_var2;
  char **t3;
  char *t1;
  char *new_var3;
  char *new_var;
  char *ret;
  t3 = (char **)(&D_00275670[arg0]);
  *((char *)(t3[4] + 3)) = 0x10;
  *((int *)(t3[4] + 4)) = 0;
  *((short *)(t3[4] + 0)) = 0xA;
  t1 = t3[4];
  ret = t1 + 0x10;
  t3[4] = t1 + 0xB0;
  *((u128 *)ret) = 0;
  *((int *)(t1 + 0x1C)) = 0x50000009;
  *((long long *)(t1 + 0x20)) = (((long long)((int)(arg3 | 4))) << 0x2F) | (0x8001 | (((long long)0x80004000) << 0x20));
  *((long long *)(t1 + 0x28)) = 0x41414141;
  func_00102948(ret + 0x20, arg2);
  func_00102948(ret + 0x30, arg1);
  func_00102948(ret + 0x40, new_var2 = arg2 + 0x10);
  func_00102948(ret + 0x50, new_var3 = arg1 + 0x10);
  func_00102948(ret + 0x60, new_var = arg2 + 0x20);
  func_00102948(ret + 0x70, new_var = arg1 + 0x20);
  func_00102948(ret + 0x80, arg2 + 0x30);
  func_00102948(ret + 0x90, arg1 + 0x30);
  return ret;
}