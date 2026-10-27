// NEARMISS func_001E3630  (vram 0x001E3630, 0x6E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.85% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0).
// NEARMISS 99.85% mwcc233 (matching wave 2026-10-27, was 93.92%). Fixes read from the
// original instructions: the self+0x14 handle is pre-added (+0x1F0) at entry and stored
// to self+0x30 (not dst+0x30), the callback goes to self+0x34; func_001AFC10 takes one
// argument (its definition); the stack buffer is 0x60 bytes; every switch case ends in
// `break` (with `return` mwcc drops the dead branch-target copies the original keeps);
// the grabbed-actor test is a one-case switch on the type byte; the retry pair is a
// switch on the side index, breaking out when func_0019A570 returns 0; dst+0xA8 is
// incremented; the wobble store goes through dst; D_00253BC8/BCC are volatile (keeps
// their store order).
// Remaining differences: one delay slot — the original leaves the abort branch
// (hit1|hit2 == 0) slot empty, mwcc 2.3.3 here copies the following scratchpad
// store's upper half into it (tried: goto/else forms, volatile stores, inline helpers).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0

// Projectile/grenade-throw launch state machine (self+4 state byte),
// dst = self+0x1F0 is the projectile's own per-instance scratch block.
// state 0: copies self+0x100 into self+0xB0, snapshots self+0xD0 into
// dst+0x20 (the launch transform), seeds self+0x30 with the owner's
// (self+0x14) +0x1F0 block and self+0x34 with the callback func_001E3620,
// zeroes self+0x20 and dst+0x14, seeds dst+0xA0/0xA4 with two PRNG-derived random
// floats in [0,1), zeroes dst+0xA8, sets self+0/4 to 1 (armed), then probes
// func_0019A570/func_0019B6C0 against (target+0xA0, self+0x100): if either
// fires, aborts the throw (dst+0x1C=0, dst+0x18=0.0f, done). Otherwise it
// computes a flight time dst+0x18 via a probe off the launch transform
// (falls through into state 1's a-priori dst+0x1C==0 case: if the aim probe
// at D_700031D0 sees a "grabbed" actor (byte 0x1A==0x5B) it commits a
// direct flight time via func_0011E748(distance^2)/50; otherwise it
// re-probes func_0019A570 with the same params, and on a hit repeats the
// same distance-based flight-time calc off the probe result (via
// func_001CD390+func_00102918), or on a miss just sets dst+0x18 = 1.0f).
// state 1: drives a particle-trail/exhaust update (func_001F0190,
// func_001CCF70/func_001CFA60/func_001CFBE0 for two visual channels),
// advances dst+0x14 by 0.04f; past 1.7f the launch is done (self=2, state
// 3); between 1.0f and 1.7f, once (dst+0xA8==0) it retries a fixed pair of
// directional probes (func_0019A570) and on double failure alerts nearby
// listeners (func_001EFD20+func_001F0460). Every tick it also steps the
// projectile position by a scaled probe delta (func_001026A0) and
// recomputes self+0x1F0 / dst+4 / dst+8 wobble offsets from self+0x20,
// finishing with func_001B17A0(self). states 2/3: forwards to
// func_001AFC10(self) (despawn/cleanup).
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *dst, void *src, void *m);
extern int func_001028D0(void *dst, void *dir, void *pos);
extern void func_00102918(void *dst, void *src, void *m);
extern void func_00102948(void *dst, void *src);
extern float func_0011E748(float x);
extern int func_00122BB8(void);
extern int func_0019A570(float *arg0, float *arg1, int arg2, int arg3);
extern int func_0019B6C0(float *arg0, float *arg1);
extern void func_001AFC10(void *self);
extern void func_001B17A0(void *self);
extern int func_001CCF70(void *p);
extern void func_001CD390(void *dst, void *src);
extern void func_001CFA60(void *sp, void *pos, float f12, float f13);
extern void func_001CFBE0(int handle, int a1, void *table, void *sp, int flag);
extern void func_001EFD20(int flags, void *p);
extern void func_001F0190(float a, float b);
extern void func_001F0290(void);
extern void func_001F0460(int a, void *p);

extern char *D_008102D0;
extern char *D_700031D0;
extern int D_00253AA0;
extern int D_00253B30;
extern int D_00253BC0;
extern int D_00253BC4;
extern volatile float D_00253BC8;
extern volatile int D_00253BCC;
extern char D_700031B0[];
extern char D_700036A0[];
extern float D_700038A0;
extern void func_001E3620(void);

void func_001E3630(unsigned char *arg0)
{
  char *target;
  unsigned char *dst;
  unsigned char *tgt;
  unsigned char st;

  tgt = *(unsigned char **)(arg0 + 0x14) + 0x1F0;
  target = D_008102D0;
  dst = arg0 + 0x1F0;
  st = arg0[4];

  switch (st) {
  case 0:
    func_00102948(arg0 + 0xB0, arg0 + 0x100);
    copy_qw4(dst + 0x20, arg0 + 0xD0);
    *(float *)(arg0 + 0x20) = 0.0f;
    *(unsigned char **)(arg0 + 0x30) = tgt;
    *(void (**)(void))(arg0 + 0x34) = func_001E3620;
    *(float *)(dst + 0x14) = 0.0f;
    *(float *)(dst + 0xA0) = (float)func_00122BB8() / 2147483648.0f;
    *(float *)(dst + 0xA4) = (float)func_00122BB8() / 2147483648.0f;
    *(int *)(dst + 0xA8) = 0;
    arg0[0] = 1;
    arg0[4] = 1;
    {
      int hit = func_0019A570((float *)(target + 0xA0), (float *)(arg0 + 0x100), 6, 0);
      hit |= func_0019B6C0((float *)(target + 0xA0), (float *)(arg0 + 0x100));
      if (hit != 0) {
        *(int *)(dst + 0x1C) = 0;
        *(float *)(dst + 0x18) = 0.0f;
        break;
      }
    }
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A8 = 50.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038A0, dst + 0x20, &D_700038A0);
    *(int *)(dst + 0x1C) = 0;
    if (func_0019B6C0((float *)(dst + 0x50), &D_700038A0) != 0) {
      switch (*(unsigned char *)(D_700031D0 + 0x1A)) {
      case 0x5B:
      {
        float d;
        func_001028D0(&D_700038A0, D_700031B0, dst + 0x50);
        d = *(float *)0x700038A0 * *(float *)0x700038A0
          + *(float *)0x700038A4 * *(float *)0x700038A4
          + *(float *)0x700038A8 * *(float *)0x700038A8;
        *(float *)(dst + 0x18) = func_0011E748(d);
        *(float *)(dst + 0x18) = *(float *)(dst + 0x18) / 50.0f;
        *(int *)(dst + 0x1C) = 0x63;
        break;
      }
      }
    }
    if (*(int *)(dst + 0x1C) == 0) {
      *(int *)(dst + 0x1C) = func_0019A570((float *)(dst + 0x50), &D_700038A0, 6, 0);
      if (*(int *)(dst + 0x1C) != 0) {
        char *probe;
        float d;
        func_001028D0(&D_700038A0, D_700031B0, dst + 0x50);
        d = *(float *)0x700038A0 * *(float *)0x700038A0
          + *(float *)0x700038A4 * *(float *)0x700038A4
          + *(float *)0x700038A8 * *(float *)0x700038A8;
        *(float *)(dst + 0x18) = func_0011E748(d);
        *(float *)(dst + 0x18) = *(float *)(dst + 0x18) / 50.0f;
        probe = D_700031D0;
        *(float *)0x700038A0 = *(float *)(probe + 0x24);
        *(float *)0x700038A4 = *(float *)(probe + 0x28);
        *(float *)0x700038A8 = *(float *)(probe + 0x2C);
        func_001CD390(dst + 0x60, &D_700038A0);
        func_00102918(dst + 0x60, dst + 0x60, D_700031B0);
      } else {
        *(float *)(dst + 0x18) = 1.0f;
      }
    }
    /* fallthrough */
  case 1:
  {
    int voice;
    int hnd;

    func_001F0190(-145.0f, 450.0f);
    voice = func_001CCF70(dst + 0x50);
    D_00253BC0 = 0x3F800000;
    D_00253BC4 = 0x3F800000;
    D_00253BC8 = 30.0f * *(float *)(dst + 0x18);
    D_00253BCC = 0x3F800000;
    {
      char sp50[0x60];
      func_001CFA60(sp50, dst + 0x20, *(float *)(dst + 0x14), *(float *)(dst + 0xA4));
      func_001CFBE0(voice, 0, &D_00253BC0, sp50, 1);

      hnd = func_001CCF70(arg0 + 0x100);
      func_001CFA60(sp50, arg0 + 0xD0, *(float *)(dst + 0x14), *(float *)(dst + 0xA0));
      func_001CFBE0(hnd, 1, &D_00253B30, sp50, 0);
      func_001CFBE0(hnd, 1, &D_00253AA0, sp50, 0);
    }
    func_001F0290();

    *(float *)(dst + 0x14) += 0.04f;
    if (*(float *)(dst + 0x14) > 1.7f) {
      arg0[0] = 2;
      arg0[4] = 3;
      break;
    }
    if (*(float *)(dst + 0x14) > 1.0f) {
      if (*(int *)(dst + 0xA8) == 0) {
        *(int *)(dst + 0xA8) += 1;
        if (*(float *)(dst + 0x18) != 1.0f && *(int *)(dst + 0x1C) == 4) {
          int side;
          for (side = 0; side < 2; side++) {
            switch (side) {
            case 0:
              *(float *)0x700038A0 = -4.0f;
              *(float *)0x700038A4 = -4.0f;
              *(float *)0x700038A8 = -2.0f;
              *(int *)0x700038AC = 0x3F800000;
              break;
            case 1:
              *(float *)0x700038A0 = 4.0f;
              *(float *)0x700038A4 = 4.0f;
              *(float *)0x700038A8 = -2.0f;
              *(int *)0x700038AC = 0x3F800000;
              break;
            }
            func_001026A0(&D_700038A0, dst + 0x60, &D_700038A0);
            if (func_0019A570((float *)(dst + 0x50), &D_700038A0, 4, 0) == 0) {
              break;
            }
          }
          if (side == 2) {
            func_001EFD20(0x8000001A, dst + 0x90);
            func_001F0460(6, dst + 0x60);
          }
        }
      }
    }

    copy_qw4(&D_700036A0, arg0 + 0xD0);
    *(int *)0x700036D0 = 0;
    *(int *)0x700036D4 = 0;
    *(int *)0x700036D8 = 0;
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A4 = 0.0f;
    *(int *)0x700036DC = 0x3F800000;
    *(float *)0x700038A8 = (50.0f * *(float *)(dst + 0x18)) / 42.500004f;
    *(int *)0x700038AC = 0x3F800000;
    func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
    *(float *)(arg0 + 0x100) = *(float *)(arg0 + 0x100) + *(float *)0x700038A0;
    *(float *)(arg0 + 0x104) = *(float *)(arg0 + 0x104) + *(float *)0x700038A4;
    *(float *)(arg0 + 0x108) = *(float *)(arg0 + 0x108) + *(float *)0x700038A8;
    func_00102948(arg0 + 0xB0, arg0 + 0x100);
    *(float *)(dst + 0) = 7.0f * (*(float *)(arg0 + 0x20) / 1.7f);
    *(float *)(dst + 4) = 7.5f * (*(float *)(arg0 + 0x20) / 1.7f);
    *(float *)(dst + 8) = 7.0f * (*(float *)(arg0 + 0x20) / 1.7f);
    *(float *)(arg0 + 0x20) += 0.04f;
    func_001B17A0(arg0);
    break;
  }

  case 2:
  case 3:
    func_001AFC10(arg0);
    break;
  }
}
