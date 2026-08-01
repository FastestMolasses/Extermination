// NEARMISS func_001DE920  (vram 0x001DE920, 0x488 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Particle/exhaust-trail VU0 packet builder (2 outer batches x 16 inner
// groups x 6 particles each). Seeds the frame-rate-scaled step at
// D_00275670+0x2450/0x2460/0x2464 via func_00102948/func_001026A0 from
// D_00810360/D_70003AC0, computes a spread scale from the byte at D_008106BF
// (converted through `unsigned int` -- that is what produces the generic
// MIPS unsigned-to-float sign-check/halve/double sequence -- then normalized
// by 127, raised to the 4th power and scaled by 4.0f), and a jittered base
// position from five func_00122BB8() draws: three integer draws scaled in
// 0.15 fixed point (u by 512, v by 224, and the "s6" jitter by 253<<20 with a
// 0x300000 bias) and two float draws in [0,1) that spread u/v by +/-spread4.
// The inner loop copies the 3x quadword camera-space basis from D_00253490
// into the stack scratch sp110, then for 6 particles writes a 0x30-byte VU0
// GIF/vertex record at the per-slot cursor (D_00275670->[0x1C]): flags word
// at +8/+4/+0 (0x80,0x80,0x80), +0xC is 0x5C on the very first particle of a
// group else 0, +0x10/+0x14 are the jittered/normalized screen-space u/v
// (using sp110's per-particle offset), +0x18=1.0f, +0x20/+0x24 are
// float_to_int(basis)-biased texture indices (<<4), +0x28 = the "s6" jitter,
// +0x2C=0. Each group's 0x150-byte record is opened with a 0x14/0x10 DMA tag,
// a quadword-zeroed header and the 0x50000013 / 0x302EC000_00008006 / 0x421
// GIF tag triple. After the 16x6 inner loop the DMA list is flushed
// (func_001D6B10/func_001D2040/func_001D6BA0/func_001D1FF0/func_001D6C90
// configure the VU0 program + GS state), and after both outer batches,
// func_001D1F20/func_001D1FF0 finalize and a final 0x60-byte record is
// appended via func_001CB760 (matching the func_001CAFA0 cursor-advance idiom).

typedef unsigned __int128 uint128;
typedef struct { uint128 a, b, c; } QW3;

extern void func_00102948(void *dst, void *src);
extern int func_001026A0(void *a0, void *a1, void *a2);
extern int func_00122BB8(void);
extern int float_to_int(float f);
extern void func_001D6B10(int a0, int a1, int a2, int a3);
extern void func_001D2040(int a0, int a1);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D6C90();
extern void func_001D1F20(int a0);
extern void func_001CB760(void *a, int b, void *c, int d);
extern char *D_00275670;
extern int D_0027568C;
extern float D_00810360[8];
extern float D_00275698;
extern char D_00253490[48];
extern char D_70003AC0[16];
extern unsigned char D_008106BF[];
extern char D_007635C0[4104];

void func_001DE920(void) {
  float sp100[4];
  union { QW3 s; float f[12]; } sp110;
  int batch, group, i;
  float baseU, baseV;
  float scale, spread4;
  char *particle;
  int jitU, jitV;
  float *basis;
  int tex;
  int jitBase;
  char *cursor;
  char *ctx;
  char *record;
  char *record0;

  func_00102948(D_00275670 + 0x2450, D_00810360);
  *(float *)(D_00275670 + 0x2460) = 2.6843546e8f / D_00275698;
  *(int *)(D_00275670 + 0x2464) = 0x3F800000;
  func_00102948(sp100, D_00275670 + 0x2450);
  func_001026A0(sp100, D_70003AC0, sp100);
  sp100[2] = (16.0f * sp100[2]) / sp100[3];
  sp100[2] = sp100[2] / 4.0f;
  scale = (float)(unsigned int)D_008106BF[0];
  spread4 = scale / 127.0f;
  spread4 *= spread4;
  spread4 *= spread4;
  spread4 *= 4.0f;
  record0 = *(char **)(D_00275670 + 0x1C);

  batch = 0;
  do {
    func_001D6B10(3, D_0027568C, 8, 8);
    func_001D2040(3, 2);
    func_001D6BA0(3, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(3, 0);
    func_001D6C90(3, 0, 1, 0, 0, 1, 0, 0, 1, 2, 0, 1, 0, 1, 0);

    group = 0;
    do {
      float randF20, randF22;

      ctx = D_00275670;
      cursor = *(char **)(ctx + 0x1C);
      cursor[3] = 0x10;
      *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
      *(short *)(*(char **)(ctx + 0x1C)) = 0x14;
      record = *(char **)(ctx + 0x1C);
      *(char **)(ctx + 0x1C) = record + 0x150;
      particle = record + 0x10;
      *(uint128 *)particle = 0;
      *(int *)(record + 0x1C) = 0x50000013;
      *(long long *)(record + 0x20) = 0x8006 | ((long long)0x302EC000 << 32);
      *(long long *)(record + 0x28) = 0x421;
      particle += 0x20;

      /* randscale idiom: each stage is a compound assignment on ONE variable,
         which gives the chain a home register instead of an expression temp. */
      jitU = func_00122BB8() >> 16;
      jitU *= 512;
      jitU >>= 15;
      jitV = func_00122BB8() >> 16;
      jitV *= 224;
      jitV >>= 15;
      {
        int t = func_00122BB8() >> 16;
        t *= 265289728;         /* 253 << 20 */
        t >>= 15;
        jitBase = t + 0x300000;
      }
      randF20 = 4.656613e-10f * (float)func_00122BB8();
      randF22 = 4.656613e-10f * (float)func_00122BB8();
      baseU = (float)jitU + (spread4 * ((2.0f * randF20) - 1.0f));
      baseV = (float)jitV + (spread4 * ((2.0f * randF22) - 1.0f));

      basis = sp110.f;
      i = 0;
      do {
        sp110.s = *(QW3 *)D_00253490;
        *(int *)(particle + 8) = 0x80;
        *(int *)(particle + 4) = 0x80;
        *(int *)(particle + 0) = 0x80;
        if (i == 0) {
          *(int *)(particle + 0xC) = 0x5C;
        } else {
          *(int *)(particle + 0xC) = 0;
        }
        *(float *)(particle + 0x10) = (baseU + basis[0]) / 512.0f;
        *(float *)(particle + 0x14) = (baseV + basis[1]) / 224.0f;
        *(int *)(particle + 0x18) = 0x3F800000;
        tex = jitU + 0x700;
        tex += float_to_int(basis[0]);
        *(int *)(particle + 0x20) = tex << 4;
        tex = jitV + 0x790;
        tex += float_to_int(basis[1]);
        *(int *)(particle + 0x24) = tex << 4;
        i += 1;
        *(int *)(particle + 0x28) = jitBase;
        *(int *)(particle + 0x2C) = 0;
        particle += 0x30;
        basis += 2;
      } while (i < 6);

      group += 1;
    } while (group < 0x10);

    batch += 1;
  } while (batch < 2);

  func_001D1F20(3);
  func_001D1FF0(3, 1);
  ctx = D_00275670;
  cursor = *(char **)(ctx + 0x1C);
  cursor[3] = 0x60;
  *(int *)(*(char **)(ctx + 0x1C) + 4) = 0;
  *(short *)(*(char **)(ctx + 0x1C)) = 0;
  *(char **)(ctx + 0x1C) = *(char **)(ctx + 0x1C) + 0x10;
  func_001CB760(D_007635C0, 0xFFF000, record0, 0x60);
}
