// NEARMISS func_001D6930  (vram 0x001D6930, 0x1D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Fixes this wave (was 64.92%): func_001D6E60 takes the caller's four arguments
// (its own definition has four parameters; the original passes a0..a3 through
// untouched); the extents 1 << arg2 / 1 << arg3 are computed before the calls and
// halved after them; the D_00275670[0x9C] flag and its 0x700 select are taken
// into locals after the cursor advance; slot spelling as in func_001D6BA0.
// Remaining differences: saved-register colouring (the original gives arg4 s0,
// the returned cursor s1, the extents s2/s3, arg0*4 s4, arg0 s5, and copies the
// return value into v0 early, reusing its register), which cascades into the
// scheduling of the header stores; the 0x700 select is a movn here vs movz in the
// original (the `flag ? 0 : 0x700` spelling gives movz but scores 81.0%).
// Declaration order, statement order, address spellings, a body local,
// -sdatathreshold 8 and a 9-minute permuter run did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// GIF/DMA sprite-packet builder. Returns the channel cursor D_00275670[arg0][4]
// read on entry. Runs func_001D6E60(arg0..arg3), func_001D2040(arg0, 0) and
// func_001D1FF0(arg0, 2), then stamps the DMAtag (byte +3 = 0x10, word +4 = 0,
// half +0 = 0xB qwords), advances the cursor by 0xC0 and fills the packet: a
// zeroed qword at +0x10, GIFtag/A+D register doublewords at +0x1C..+0x68 (the
// +0x40 word ORs in 0x700 when D_00275670[0x9C] == 0), the 16-byte block at arg4
// copied to +0x70, and scissor/extent words at +0x80..+0xBC from the
// half-extents hx = (1 << arg2) >> 1, hy = (1 << arg3) >> 1.
typedef unsigned __int128 u128;
extern char **D_00275670;
extern char *func_001D6E60(int arg0, int arg1, int arg2, int arg3);
extern void func_001D2040(int ch, int idx);
extern void func_001D1FF0(int ch, int idx);

char *func_001D6930(int arg0, int arg1, int arg2, int arg3, char * arg4)
{
  char *ret;
  int w;
  int h;
  int flag;
  int m;
  char *t1;
  char **t3;
  ret = ((char **)(&D_00275670[arg0]))[4];
  w = 1 << arg2;
  h = 1 << arg3;
  func_001D6E60(arg0, arg1, arg2, arg3);
  func_001D2040(arg0, 0);
  func_001D1FF0(arg0, 2);
  w >>= 1;
  t3 = (char **)(&D_00275670[arg0]);
  *(char *)(t3[4] + 3) = 0x10;
  *(int *)(t3[4] + 4) = 0;
  *(short *)(t3[4] + 0) = 0xB;
  t1 = t3[4];
  t3[4] = t1 + 0xC0;
  h >>= 1;
  flag = *(int *)((char *)D_00275670 + 0x9C);
  m = (flag == 0) ? 0x700 : 0;
  *(u128 *)(t1 + 0x10) = 0;
  *(int *)(t1 + 0x1C) = 0x5000000A;
  *(long long *)(t1 + 0x20) = 0x8003LL | ((long long)0x10000000 << 0x20);
  *(long long *)(t1 + 0x28) = 0xE;
  *(long long *)(t1 + 0x30) = 0;
  *(long long *)(t1 + 0x38) = 0x3F;
  *(long long *)(t1 + 0x40) = (long long)m | (0x24020000 | ((long long)0xA << 0x20));
  *(long long *)(t1 + 0x48) = 6;
  *(long long *)(t1 + 0x50) = 0x20;
  *(long long *)(t1 + 0x58) = 0x3B;
  *(long long *)(t1 + 0x60) = 0x8001LL | ((long long)0x508B4000 << 0x20);
  *(long long *)(t1 + 0x68) = 0x43431;
  *(u128 *)(t1 + 0x70) = *(u128 *)arg4;
  *(int *)(t1 + 0x80) = 8;
  *(int *)(t1 + 0x84) = 8;
  *(int *)(t1 + 0x90) = (0x800 - w) * 0x10;
  *(int *)(t1 + 0x94) = (0x800 - h) * 0x10;
  *(int *)(t1 + 0x98) = 0;
  *(int *)(t1 + 0x9C) = 0xFF;
  *(int *)(t1 + 0xA0) = 0x1FF8;
  *(int *)(t1 + 0xA4) = 0xDF8;
  *(int *)(t1 + 0xB0) = (w + 0x800) * 0x10;
  *(int *)(t1 + 0xB4) = (h + 0x800) * 0x10;
  *(int *)(t1 + 0xB8) = 0;
  *(int *)(t1 + 0xBC) = 0xFF;
  return ret;
}
