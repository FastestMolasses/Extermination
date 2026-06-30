// NEARMISS func_001D6930  (vram 0x001D6930, 0x1D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 64.92% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// pervasive regalloc-coloring near-miss (64.9% mwcc233): instruction sequence matches but target colors block-ptr->t4, arg0*4->s4, cursor->s1/early-v0, arg4->s0 while mwcc picks block-ptr->a0, arg0*4->s0, cursor->s5, arg4->s1; plus movz-vs-movn on the [0x9C]==0 conditional and early-vs-late return ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 64.9% (mwcc 2.3.3; 991202=45.4%). GIF/DMA sprite-packet builder.
// Returns the current write cursor *(D_00275670 + arg0*4 + 0x10). Runs
// func_001D6E60(), then func_001D2040(arg0,0) and func_001D1FF0(arg0,2). hx/hy
// are half-extents (1<<arg2)>>1 / (1<<arg3)>>1. Through dl = D_00275670+arg0*4 it
// stamps a small tag header into the cursor (byte+3=0x10, word+4=0, half+0=0xB),
// advances the cursor by 0xC0, then fills a 0xC0-byte GIF packet at the old
// cursor: a zeroed qword at +0x10, GIFtag/AD register doublewords at +0x1C..+0x68
// (the +0x40 word ORs in 0x700 when D_00275670[0x9C]==0), the 16-byte source
// block at arg4 copied to +0x70, scissor/extent words at +0x80..+0xBC derived
// from hx/hy (e.g. (0x800-hx)*0x10, (hx+0x800)*0x10).
//
// Body + the full store sequence are recovered and instruction-order-identical;
// the residual is pervasive CW-vs-mwcc register coloring (target colors the
// block ptr->t4, arg0*4->s4, cursor->s1/early-v0, arg4->s0; mwcc picks
// block ptr->a0, arg0*4->s0, cursor->s5, arg4->s1) plus a movz-vs-movn choice on
// the D_00275670[0x9C]==0 conditional and an early-vs-late return-value move.
// All register-allocation-order artifacts, not the clean-store nop -> permuter.
typedef unsigned __int128 uint128;
extern char *D_00275670;
extern int func_001D6E60(void);
extern void func_001D2040(int a, int b);
extern void func_001D1FF0(int a, int b);

int func_001D6930(int arg0, int arg1, int arg2, int arg3, int arg4)
{
  char *dl;
  char *cur;
  int cursor;
  int hx;
  int hy;
  int self;
  int src;

  cursor = *(int *)(D_00275670 + arg0 * 4 + 0x10);
  src = arg4;
  func_001D6E60();
  self = arg0;
  func_001D2040(self, 0);
  func_001D1FF0(self, 2);
  hx = (1 << arg2) >> 1;
  dl = D_00275670 + arg0 * 4;
  hy = (1 << arg3) >> 1;
  *(char *)(*(int *)(dl + 0x10) + 3) = 0x10;
  *(int *)(*(int *)(dl + 0x10) + 4) = 0;
  *(short *)(*(int *)(dl + 0x10) + 0) = 0xB;
  cur = *(char **)(dl + 0x10);
  *(int *)(dl + 0x10) = (int)(cur + 0xC0);
  *(uint128 *)(cur + 0x10) = 0;
  *(unsigned int *)(cur + 0x1C) = 0x5000000A;
  *(long *)(cur + 0x20) = (long)0x8003 | ((long)0x10000000 << 32);
  *(long *)(cur + 0x28) = 0xE;
  *(long *)(cur + 0x30) = 0;
  *(long *)(cur + 0x38) = 0x3F;
  *(long *)(cur + 0x40) = ((long)(*(int *)(D_00275670 + 0x9C) == 0 ? 0x700 : 0)) | (0x24020000 | ((long)0xA << 32));
  *(long *)(cur + 0x48) = 6;
  *(long *)(cur + 0x50) = 0x20;
  *(long *)(cur + 0x58) = 0x3B;
  *(long *)(cur + 0x60) = (long)0x8001 | ((long)0x508B4000 << 32);
  *(long *)(cur + 0x68) = 0x43431;
  *(uint128 *)(cur + 0x70) = *(uint128 *)(src + 0x0);
  *(int *)(cur + 0x80) = 8;
  *(int *)(cur + 0x84) = 8;
  *(int *)(cur + 0x90) = (0x800 - hx) * 0x10;
  *(int *)(cur + 0x94) = (0x800 - hy) * 0x10;
  *(int *)(cur + 0x98) = 0;
  *(int *)(cur + 0x9C) = 0xFF;
  *(int *)(cur + 0xA0) = 0x1FF8;
  *(int *)(cur + 0xA4) = 0xDF8;
  *(int *)(cur + 0xB0) = (hx + 0x800) * 0x10;
  *(int *)(cur + 0xB4) = (hy + 0x800) * 0x10;
  *(int *)(cur + 0xB8) = 0;
  *(int *)(cur + 0xBC) = 0xFF;
  return cursor;
}
