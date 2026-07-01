// NEARMISS func_001B6FA0  (vram 0x001B6FA0, 0x3FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 88.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 88.19% mwcc233 (991202 84.12%). Fully recovered 5-state anim blend dispatcher: all call args (anim_clip_init, func_001B1240/12B0/1380/1470), saved-reg mapping (arg0->s2, arg2->s0, arg1->s1), and all float constants (20.0f, 30.0f, plus raw int bit-patterns for the D_008104A8/A4 stores) ve...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void anim_clip_init(void *self, int clip, float a, float b);
extern int func_001B1380(void *a, void *b, float c);
extern float func_001B1240(void *a, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(int a1, float d);
extern void *D_00810350;
extern float D_00810360;
extern float D_00810368;
extern float D_00810374;
extern short D_008104A2;
extern int D_008104A4;
extern int D_008104A8;
extern int D_008104B0;
extern int D_002821B0;
extern int D_002821B4;
extern int D_002821B8;
extern int D_002821BC;

int func_001B6FA0(char *arg0, char *arg1, char *arg2)
{
  unsigned char st;
  int v0;
  float f20, f21;
  float v;

  st = arg1[4];
  switch (st) {
  case 0:
    *(float *)(arg2 + 0x10) = *(float *)(arg0 + 0xC4);
    if (func_001B1380(&D_00810350, arg0 + 0xB0, *(float *)(arg0 + 0xC4)) != 0) {
      anim_clip_init(arg0, *(short *)(arg2 + 0x18), 20.0f, 0.0f);
    } else {
      anim_clip_init(arg0, *(short *)(arg2 + 0x14), 20.0f, 0.0f);
    }
    if (func_001B1380(arg0 + 0xB0, &D_00810350, D_00810374) != 0) {
      v0 = 0x156;
    } else {
      v0 = 0x155;
    }
    D_008104A2 = v0;
    D_008104A8 = 0x41A00000;
    D_008104A4 = 0x3F800000;
    arg1[4] = 1;
    return 0;

  case 1:
    if (*(float *)(arg2 + 0x24) == 0.0f) {
      f20 = *(float *)(arg0 + 0xC4);
    } else {
      f20 = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
      *(float *)(arg0 + 0xC4) = func_001B12B0(f20, *(float *)(arg0 + 0xC4), *(float *)(arg2 + 0x24));
    }
    if (*(float *)(arg2 + 0x34) == 0.0f) {
      f21 = D_00810374;
    } else {
      f21 = func_001B1240(&D_00810360, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8));
      D_00810374 = func_001B12B0(f21, D_00810374, *(float *)(arg2 + 0x34));
    }
    if (D_00810374 == f21) {
      D_008104A2 = 0x164;
      D_008104A8 = 0x41F00000;
    }
    if ((*(float *)(arg0 + 0xC4) == f20) && (D_00810374 == f21)) {
      D_008104A2 = 0x166;
      D_008104A8 = 0x41F00000;
      anim_clip_init(arg0, *(short *)(arg2 + 0x1C), 30.0f, 0.0f);
      D_002821B0 = 2;
      D_002821B4 = 1;
      D_002821B8 = *(int *)(arg2 + 8);
      D_002821BC = 0;
      arg1[4] = 2;
    }
    return 0;

  case 2:
    if (D_002821B4 != 2) {
      if (D_002821B0 != 0) {
        return 0;
      }
    }
    D_008104A2 = 0x165;
    D_008104A8 = 0x41F00000;
    if (func_001B1470(st, *(float *)(arg0 + 0xC4) - *(float *)(arg2 + 0x10)) < 0.0f) {
      anim_clip_init(arg0, *(short *)(arg2 + 0x18), 20.0f, 0.0f);
    } else {
      anim_clip_init(arg0, *(short *)(arg2 + 0x14), 20.0f, 0.0f);
    }
    arg1[4] = 3;
    return 0;

  case 3:
  {
    float t24 = *(float *)(arg2 + 0x24);
    if (t24 == 0.0f) {
      v = *(float *)(arg2 + 0x10);
    } else {
      v = func_001B12B0(*(float *)(arg2 + 0x10), *(float *)(arg0 + 0xC4), t24);
    }
    *(float *)(arg0 + 0xC4) = v;
    if (*(float *)(arg0 + 0xC4) == *(float *)(arg2 + 0x10)) {
      anim_clip_init(arg0, *(short *)(arg2 + 4), 20.0f, 0.0f);
      if (D_008104B0 & 0x1000) {
        return 1;
      }
      arg1[4] = 4;
      return 0;
    }
    return 0;
  }

  case 4:
    if (D_008104B0 & 0x1000) {
      return 1;
    }
    return 0;
  }
  return 0;
}
