// NEARMISS func_001DE920  (vram 0x001DE920, 0x488 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Full VU0/GS particle (exhaust-trail) packet-builder logic recovered (2 batches x 16 groups x 6 particles), confirmed by matching instruction count (341/341) with the target. Not VU0 macro-mode -- this is plain scalar C like its sibling func_001D40E0/func_001CAFA0. Residuals are (1) genuine regist...
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
// D_00810360/D_70003AC0, computes a spread scale from D_008106BF (a signed
// byte read as an unsigned "fold" -- negative values are folded via
// ((v>>1)|(v&1))*2 -- normalized by 127 then raised to the 4th power and
// scaled by 4.0f), and a size/speed pair (temp_f20/f22) from four
// func_00122BB8() draws combined with the "s2/s3/s6" jittered base position.
// The inner loop copies the 3x quadword camera-space basis from
// D_00253490 into the stack scratch sp110, then for 6 particles writes a
// 0x30-byte VU0 GIF/vertex record at the per-slot cursor
// (D_00275670->[0x1C]): flags word at +8/+4/+0 (0x80,0x80,0x80), +0xC is
// 0x5C on the very first particle of a batch else 0, +0x10/+0x14 are the
// jittered/normalized screen-space u/v (using sp110's per-particle offset),
// +0x18=1.0f, +0x20/+0x24 are float_to_int(128*uv)-biased texture indices
// (<<4), +0x28=temp_s6 (the "s6" jitter), +0x2C=0. After the 16x6 inner
// loop the DMA list is flushed (func_001D6B10/func_001D2040/func_001D6BA0/
// func_001D1FF0/func_001D6C90 configure the VU0 program + GS state), and
// after both outer batches, func_001D1F20/func_001D1FF0 finalize and a
// final 0x60-byte record is appended via func_001CB760 (matching the
// func_001CAFA0 cursor-advance idiom).
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
extern unsigned char D_008106BF;
extern char D_007635C0[4104];

void func_001DE920(void) {
  float sp100[4];
  float sp110[12];
  int batch, group, i;
  float scale, spread2, spread4;
  int jitU, jitV, jitBase;
  float baseU, baseV;
  char *cursor;
  char *record;
  char *record0;
  char *particle;
  float *basis;

  func_00102948(D_00275670 + 0x2450, D_00810360);
  *(float *)(D_00275670 + 0x2460) = 2.6843546e8f / D_00275698;
  *(int *)(D_00275670 + 0x2464) = 0x3F800000;
  func_00102948(sp100, D_00275670 + 0x2450);
  func_001026A0(sp100, D_70003AC0, sp100);
  sp100[2] = (16.0f * sp100[2]) / sp100[3];
  sp100[2] = sp100[2] / 4.0f;
  if (D_008106BF < 0) {
    scale = 2.0f * (float)((D_008106BF >> 1) | (D_008106BF & 1));
  } else {
    scale = (float)D_008106BF;
  }
  spread2 = scale / 127.0f;
  spread2 = spread2 * spread2;
  spread4 = spread2 * spread2 * 4.0f;
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

      cursor = *(char **)(D_00275670 + 0x1C);
      cursor[3] = 0x10;
      *(int *)(*(char **)(D_00275670 + 0x1C) + 4) = 0;
      *(short *)(*(char **)(D_00275670 + 0x1C)) = 0x14;
      record = *(char **)(D_00275670 + 0x1C);
      *(char **)(D_00275670 + 0x1C) = record + 0x150;
      *(long long *)(record + 0x10) = 0;
      particle = record + 0x30;
      *(int *)(record + 0x1C) = 0x50000013;
      *(long long *)(record + 0x20) = 0x8006 | ((long long)0x302EC000 << 32);
      *(long long *)(record + 0x28) = 0x421;

      jitU = ((func_00122BB8() >> 16) * 512) >> 15;
      jitV = ((func_00122BB8() >> 16) * 0xE0) >> 15;
      jitBase = ((((func_00122BB8() >> 16) * 253) * 32)) + 0x300000;
      randF20 = 4.656613e-10f * (float)func_00122BB8();
      randF22 = 4.656613e-10f * (float)func_00122BB8();
      baseU = (float)jitU + (spread4 * (-1.0f + (2.0f * randF20)));
      baseV = (float)jitV + (spread4 * (-1.0f + (2.0f * randF22)));

      basis = sp110;
      i = 0;
      do {
        *(long long *)&sp110[0] = *(long long *)&D_00253490[0];
        *(long long *)&sp110[2] = *(long long *)&D_00253490[2];
        *(long long *)&sp110[4] = *(long long *)&D_00253490[4];
        *(long long *)&sp110[6] = *(long long *)&D_00253490[6];
        *(long long *)&sp110[8] = *(long long *)&D_00253490[8];
        *(long long *)&sp110[10] = *(long long *)&D_00253490[10];
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
        *(int *)(particle + 0x20) = (jitU + 0x700 + float_to_int(basis[0])) << 4;
        *(int *)(particle + 0x24) = (jitV + 0x790 + float_to_int(basis[1])) << 4;
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
  cursor = *(char **)(D_00275670 + 0x1C);
  cursor[3] = 0x60;
  *(int *)(cursor + 4) = 0;
  *(short *)cursor = 0;
  *(char **)(D_00275670 + 0x1C) = cursor + 0x10;
  func_001CB760(D_007635C0, 0xFFF000, record0, 0x60);
}
