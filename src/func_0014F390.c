// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Enemy limb/appendage swing state machine (2 states on self+6). State 0: seeds the
// partner record e (+4/+8 flags, +0x64..0x67 bytes) from a 0x44-stride tuning table
// (D_00245D20.. columns) picked by D_00245EC0[e[0x65]*16 + rand-nibble], converts the
// D_00245D20 angle to radians into self+0xC4, randomizes 4 per-limb phase bytes
// (e+0x68..) from func_00122BB8() bits, then anim_clip_init + pose floats at
// self+0xB0/B4/B8/BC. State 1: dual clamp/oscillator update on (e+0x2C,e+0x30,e+0x34)
// and (e+0x40,e+0x44,e+0x48) driven by self+0xB4 vs D_00245D58/D_00245D5C thresholds,
// accumulates swing into self+0xB0, flip logic (n==0 paths) reverses direction with
// -1.0f/-1.4f/-2.0f factors, calls func_00151200, plays sounds 0x872/0x871/0x879 via
// func_001FBD50 on crossing +/-90..95 degrees, and resets to state 0 when flag 0x1000
// and self+0xB4 <= -160. Verified objdiff 100% (mwcc 2.3.3) vs build/expected/func_0014F390.o;
// 991202 walls at 91.5% (idiom-13 delay-slot nops + unsigned-char andi artifacts).

extern int anim_clip_init(unsigned char *self, int a1, int a2, float f12, float f13);
extern int func_00122BB8();
extern int func_00151200(unsigned char *p, unsigned char *e);
extern int func_001FBD50(unsigned char *e, int idx, int a, float b);

extern float D_00245D20[];
extern float D_00245D24[];
extern float D_00245D28[];
extern float D_00245D2C[];
extern float D_00245D30[];
extern float D_00245D34[];
extern float D_00245D38[];
extern float D_00245D3C[];
extern float D_00245D40[];
extern float D_00245D44[];
extern float D_00245D48[];
extern float D_00245D4C[];
extern float D_00245D50[];
extern float D_00245D54[];
extern float D_00245D58[];
extern float D_00245D5C[];
extern signed char D_00245D60[];
extern signed char D_00245EC0[];

void func_0014F390(unsigned char *p, unsigned char *e)
{
  unsigned char st;
  int i;
  unsigned char *q;

  st = p[6];
  switch (st) {
  case 0:
    p[6] = st + 1;
    *(int *)(e + 4) = 0;
    *(int *)(e + 8) = 0;
    *(signed char *)(e + 0x64) = 6;
    *(signed char *)(e + 0x67) = 0;
    *(signed char *)(e + 0x66) =
        D_00245EC0[*(signed char *)(e + 0x65) * 16 + ((func_00122BB8() >> 12) & 0xF)];
    *(float *)(e + 0x2C) = D_00245D34[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x30) = D_00245D38[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x34) = 0.0f;
    *(float *)(e + 0x38) = D_00245D40[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x3C) = D_00245D48[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x40) = D_00245D4C[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x44) = D_00245D50[*(signed char *)(e + 0x66) * 17];
    *(float *)(e + 0x48) = 0.0f;
    *(float *)(p + 0xC4) =
        (3.1415927f * D_00245D20[*(signed char *)(e + 0x66) * 17]) / 180.0f;
    for (i = 0; i < 4; i++)
    {
      e[i + 0x6C] = 0;
      e[i + 0x68] = (func_00122BB8() >> (i + 9)) & 7;
    }
    q = *(unsigned char **)(p + 0x20);
    if (q)
    {
      q[5] = 1;
    }
    anim_clip_init(p, 1, *(signed char *)(e + 0x66), 0.0f,
                   D_00245D24[*(signed char *)(e + 0x66) * 17]);
    *(float *)(p + 0xB0) = D_00245D28[*(signed char *)(e + 0x66) * 17];
    *(float *)(p + 0xB4) = D_00245D2C[*(signed char *)(e + 0x66) * 17];
    *(float *)(p + 0xB8) = D_00245D30[*(signed char *)(e + 0x66) * 17];
    *(float *)(p + 0xBC) = 1.0f;
    break;
  case 1:
  {
    float dv, m;
    int n, fl;

    dv = *(float *)(e + 0x34);
    if (!(dv <= 0.0f))
    {
      if (!(*(float *)(e + 0x2C) < *(float *)(e + 0x30)))
      {
        *(float *)(e + 0x2C) = *(float *)(e + 0x30);
        *(float *)(e + 0x34) = 0.0f;
      }
    }
    else if (dv < 0.0f)
    {
      if (*(float *)(e + 0x2C) <= *(float *)(e + 0x30))
      {
        *(float *)(e + 0x2C) = *(float *)(e + 0x30);
        *(float *)(e + 0x34) = 0.0f;
      }
    }
    else
    {
      if (!(*(float *)(p + 0xB4) < D_00245D58[*(signed char *)(e + 0x66) * 17]))
      {
        *(float *)(e + 0x34) = D_00245D3C[*(signed char *)(e + 0x66) * 17];
      }
    }

    dv = *(float *)(e + 0x34);
    dv = *(float *)(e + 0x2C) + dv;
    *(float *)(e + 0x2C) = dv;
    *(float *)(p + 0xB0) = *(float *)(p + 0xB0) + dv;

    m = D_00245D44[*(signed char *)(e + 0x66) * 17];
    if (*(float *)(e + 0x38) < m)
    {
      *(float *)(e + 0x38) = m;
      *(float *)(e + 0x3C) = 0.0f;
    }

    if (*(float *)(e + 0x38) < 0.0f && !(*(signed char *)(e + 0x67) & 0x80))
    {
      *(signed char *)(e + 0x67) = -0x80;
      q = *(unsigned char **)(p + 0x20);
      if (q)
      {
        q[5] = 2;
      }
    }

    dv = *(float *)(e + 0x48);
    if (!(dv <= 0.0f))
    {
      if (!(*(float *)(e + 0x40) < *(float *)(e + 0x44)))
      {
        *(float *)(e + 0x40) = *(float *)(e + 0x44);
        *(float *)(e + 0x48) = 0.0f;
      }
    }
    else if (dv < 0.0f)
    {
      if (*(float *)(e + 0x40) <= *(float *)(e + 0x44))
      {
        *(float *)(e + 0x40) = *(float *)(e + 0x44);
        *(float *)(e + 0x48) = 0.0f;
      }
    }
    else
    {
      if (!(*(float *)(p + 0xB4) < D_00245D5C[*(signed char *)(e + 0x66) * 17]))
      {
        *(float *)(e + 0x48) = D_00245D54[*(signed char *)(e + 0x66) * 17];
      }
    }

    n = *(int *)(e + 4);
    if (n == 0 && D_00245D60[*(signed char *)(e + 0x66) * 68] == 2)
    {
      float f2 = *(float *)(e + 0x2C);
      if (!(f2 <= 0.0f) && !(*(float *)(e + 0x34) < 0.0f) &&
          !(*(float *)(p + 0xB0) < 90.0f))
      {
        *(int *)(e + 4) = 1;
        *(float *)(e + 0x30) = -1.0f * *(float *)(e + 0x30);
        *(float *)(e + 0x34) = -1.4f * D_00245D3C[*(signed char *)(e + 0x66) * 17];
      }
      else if (f2 < 0.0f)
      {
        if (*(float *)(e + 0x34) <= 0.0f && *(float *)(p + 0xB0) <= 110.0f)
        {
          *(int *)(e + 4) = 1;
          *(float *)(e + 0x30) = -1.0f * *(float *)(e + 0x30);
          *(float *)(e + 0x34) = -1.4f * D_00245D3C[*(signed char *)(e + 0x66) * 17];
        }
      }
    }
    else if (n == 0 && *(signed char *)(e + 0x65) == 2 &&
             !(*(float *)(p + 0xB8) < 140.0f))
    {
      *(int *)(e + 4) = 1;
      *(float *)(e + 0x44) = -2.0f * *(float *)(e + 0x44);
      *(float *)(e + 0x48) = -2.0f * D_00245D54[*(signed char *)(e + 0x66) * 17];
      *(float *)(e + 0x34) = D_00245D3C[*(signed char *)(e + 0x66) * 17];
    }
    func_00151200(p, e);

    if (*(float *)(e + 0x38) < 0.0f)
    {
      fl = *(int *)(e + 8);
      if (!(fl & 2) && *(float *)(p + 0xB4) <= 90.0f)
      {
        *(int *)(e + 8) = fl | 2;
        func_001FBD50(p, 0x872, 0, 1000.0f);
      }
    }
    else
    {
      fl = *(int *)(e + 8);
      if (!(fl & 1) && !(*(float *)(p + 0xB4) < -95.0f))
      {
        *(int *)(e + 8) = fl | 1;
        func_001FBD50(p, 0x871, 0, 1000.0f);
        func_001FBD50(p, 0x879, 0, 800.0f);
      }
    }

    if ((*(int *)(e + 0) & 0x1000) && *(float *)(p + 0xB4) <= -160.0f)
    {
      p[5] = 0;
      p[6] = 0;
      *(signed char *)(e + 0x64) = 0;
      *(float *)(p + 0xC4) = 0.0f;
      *(signed char *)(e + 0x65) = D_00245D60[*(signed char *)(e + 0x66) * 68];
    }
    break;
  }
  }
}
