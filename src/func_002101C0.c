// NEARMISS func_002101C0  (vram 0x002101C0, 0x83C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.91% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 89.09% mwcc/991202 (233 87.91% - target FILLS branch slots with lui/li, so 991202 is the lane here). LOGIC 100% recovered and verified block-by-block: 4-way switch on p[4] (0=model init via func_001C6120/func_001CA5E0 + bone array alloc loop with gp-rel D_00275BCC limit check, fallthroug...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Camera/viewport controller state machine on p[4]. State 0 (init): if the slot
// flag D_00810CB8[p[3]] is set, resolves the display model via func_001C6120 from
// D_002658C0/C4 (8-byte stride, picked by split-screen flag p[0xD]) and binds it
// with func_001CA5E0(p, model, 7); allocates p[0xC] = func_001C6150(*(p+0x44)) bone
// slots (aborts to state 3 if over the gp-rel budget D_00275BCC), filling p+0x110[]
// from func_001AF7C0, then anim_bone_array_setup + bone_init_default_1. Seeds view
// scales (p+0x80..0x8C, 0.5f defaults), rates 0.10666667f (=16/150) at p+0x60..0x68,
// and yaw/pitch p+0xA0/A8 from D_00265600/04, then falls into state 1.
// State 1 (per-frame): picks view scale set by focus (D_008106CD low nibble) and
// active-player (D_00810142) match: 0.55f/0.15f/2.0f/0.5f; if D_00810143 (zoom mode)
// is 0 resets rates and re-binds via func_00210030(p,0)+func_001C6380+vtable call
// (*(p+0x4C))(p); otherwise processes pad bits D_00810E70 (2/8 = zoom D_00810154 in
// [2,10]; 0x2000/0x8000 = pitch D_00810158 clamped to D_002657B0/B4 row p[3]*5;
// 0x1000/0x4000 = yaw D_0081015C clamped to D_002657B8/BC), scales rates by zoom and
// re-binds with func_00210030(p, D_00810143). Tail (not split-screen, not menu 0x12/
// 0x15, focused, zooming): computes camera-to-target offset D_00810170/178 from the
// player position row D_00265890[p[3]] (float[3] stride) vs camera at D_008102B0+0xA0/
// 0xA8, rotates it through scratchpad vector 0x700038A0 with matrix D_700036A0 via
// func_001029C0(identity/rot by D_00810144)+func_00102BB0(-row z)+func_001026A0, then
// applies zoom-scaled pitch/yaw drift (0.10666667f/0.08533333f factors).
// States 2/3/default: func_001AFF90(p) teardown.
// D_00275BCC is gp-relative (sdatathreshold 8); all other globals array-declared to
// force absolute %hi/%lo addressing.

extern int func_001026A0(float *dst, float *src, float *m);
extern int func_001029C0(float *m, int idx);
extern int func_00102BB0(float *a, float *b, float f);
extern int func_001AF7C0(void);
extern int func_001AFF90(unsigned char *p);
extern int func_001C6120(int model, int id);
extern unsigned char func_001C6150(int v);
extern int func_001C6380(unsigned char *p);
extern int func_001CA5E0(unsigned char *p, int model, int mode);
extern int func_00210030(unsigned char *p, int a1);
extern int anim_bone_array_setup(int n);
extern int bone_init_default_1(unsigned char *p);

extern short D_00275BCC;            /* gp-relative small data */
extern int D_0028A570[];
extern float D_00265600[];
extern float D_00265604[];
extern float D_002657B0[];
extern float D_002657B4[];
extern float D_002657B8[];
extern float D_002657BC[];
extern float *D_00265890[];
extern int D_002658C0[];
extern int D_002658C4[];
extern unsigned char D_00810142[];
extern unsigned char D_00810143[];
extern unsigned char D_00810144[];
extern float D_00810154[];
extern float D_00810158[];
extern float D_0081015C[];
extern float D_00810170[];
extern float D_00810174[];
extern float D_00810178[];
extern float D_008102B0[];
extern unsigned char D_008106CD[];
extern unsigned char D_00810700[];
extern unsigned char D_00810CB8[];
extern unsigned short D_00810E70[];
extern float D_700036A0[];
extern float D_700038A0[];

void func_002101C0(unsigned char *p)
{
  float *cam = D_008102B0;
  unsigned char st;
  int i, n;

  st = p[4];
  switch (st) {
  case 0:
    i = p[3];
    if (D_00810CB8[i])
    {
      if (p[0xD] == 0)
      {
        int t = D_002658C0[i * 2];
        func_001CA5E0(p, func_001C6120(D_0028A570[0], t), 7);
      }
      else
      {
        int t = D_002658C4[i * 2];
        func_001CA5E0(p, func_001C6120(D_0028A570[0], t), 7);
      }
      p[0xC] = func_001C6150(*(int *)(p + 0x44));
      if (D_00275BCC < (int)p[0xC])
      {
        p[4] = 3;
        return;
      }
      for (n = 0; n < p[0xC]; n++)
      {
        *(int *)(p + 0x110 + n * 4) = func_001AF7C0();
      }
      p[9] = p[0xC];
      anim_bone_array_setup(p[0xC]);
      bone_init_default_1(p);
    }
    p[2] = p[2] | 0x40;
    if (p[0xD] == 0)
    {
      *(float *)(p + 0x80) = 0.0f;
      *(float *)(p + 0x84) = 0.0f;
      *(float *)(p + 0x88) = 0.5f;
    }
    else
    {
      *(float *)(p + 0x80) = 0.5f;
      *(float *)(p + 0x84) = 0.5f;
      *(float *)(p + 0x88) = 0.5f;
    }
    *(float *)(p + 0x8C) = 0.0f;
    p[4] = 1;
    *(float *)(p + 0xC0) = 0.0f;
    *(float *)(p + 0xC4) = 0.0f;
    *(float *)(p + 0xC8) = 0.0f;
    *(float *)(p + 0x60) = 0.10666667f;
    *(float *)(p + 0x64) = 0.10666667f;
    *(float *)(p + 0x68) = 0.10666667f;
    *(float *)(p + 0xA0) = -D_00265600[p[3] * 2];
    *(float *)(p + 0xA8) = D_00265604[p[3] * 2];
    *(float *)(p + 0xA4) = 0.0f;
    /* fallthrough */
  case 1:
    if (p[0xD] == 0)
    {
      if ((D_008106CD[0] & 0xF) == p[3])
      {
        if (D_00810142[0] == p[3])
        {
          *(float *)(p + 0x80) = 0.0f;
          *(float *)(p + 0x84) = 0.55f;
          *(float *)(p + 0x88) = 0.55f;
        }
        else
        {
          *(float *)(p + 0x80) = 0.0f;
          *(float *)(p + 0x84) = 0.15f;
          *(float *)(p + 0x88) = 0.15f;
        }
      }
      else if (D_00810142[0] == p[3])
      {
        *(float *)(p + 0x80) = 0.0f;
        *(float *)(p + 0x84) = 0.0f;
        *(float *)(p + 0x88) = 2.0f;
      }
      else
      {
        *(float *)(p + 0x80) = 0.0f;
        *(float *)(p + 0x84) = 0.0f;
        *(float *)(p + 0x88) = 0.5f;
      }
    }
    else
    {
      if (D_00810142[0] != p[3])
      {
        return;
      }
      *(float *)(p + 0x80) = 2.0f;
      *(float *)(p + 0x84) = 2.0f;
      *(float *)(p + 0x88) = 2.0f;
    }

    if (D_00810143[0] == 0)
    {
      *(float *)(p + 0xC8) = 0.0f;
      *(float *)(p + 0x60) = 0.10666667f;
      *(float *)(p + 0x64) = 0.10666667f;
      *(float *)(p + 0x68) = 0.10666667f;
      if (D_00810CB8[p[3]])
      {
        func_00210030(p, 0);
        func_001C6380(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
      }
    }
    else
    {
      if (D_00810CB8[p[3]] && D_00810142[0] == (p[3] & 0xFF))
      {
        if (p[0xD] == 0)
        {
          int pad = D_00810E70[0];
          float f, lim;

          if (pad & 2)
          {
            f = D_00810154[0] - 0.1f;
            D_00810154[0] = f;
            if (f < 2.0f)
            {
              D_00810154[0] = 2.0f;
            }
          }
          else if (pad & 8)
          {
            f = D_00810154[0] + 0.1f;
            D_00810154[0] = f;
            if (!(f <= 10.0f))
            {
              D_00810154[0] = 10.0f;
            }
          }

          if (pad & 0x2000)
          {
            D_00810158[0] = D_00810158[0] + 2.0f;
            lim = D_002657B4[p[3] * 5];
            if (lim < D_00810158[0])
            {
              D_00810158[0] = lim;
            }
          }
          else if (pad & 0x8000)
          {
            D_00810158[0] = D_00810158[0] - 2.0f;
            lim = D_002657B0[p[3] * 5];
            if (!(lim <= D_00810158[0]))
            {
              D_00810158[0] = lim;
            }
          }

          if (pad & 0x1000)
          {
            D_0081015C[0] = D_0081015C[0] - 2.0f;
            lim = D_002657B8[p[3] * 5];
            if (!(lim <= D_0081015C[0]))
            {
              D_0081015C[0] = lim;
            }
          }
          else if (pad & 0x4000)
          {
            D_0081015C[0] = D_0081015C[0] + 2.0f;
            lim = D_002657BC[p[3] * 5];
            if (lim < D_0081015C[0])
            {
              D_0081015C[0] = lim;
            }
          }
        }
        *(float *)(p + 0x60) = 0.10666667f * D_00810154[0];
        *(float *)(p + 0x64) = 0.10666667f * D_00810154[0];
        *(float *)(p + 0x68) = 0.10666667f * D_00810154[0];
        *(float *)(p + 0xC8) = 0.0f;
        func_00210030(p, D_00810143[0]);
        func_001C6380(p);
        (*(void (**)(unsigned char *))(p + 0x4C))(p);
      }
    }

    if (p[0xD] != 1 && D_00810700[0] != 0x12 && D_00810700[0] != 0x15)
    {
      if ((D_008106CD[0] & 0xF) == p[3] && D_00810143[0] != 0)
      {
        float *row;
        float half, k;

        row = D_00265890[p[3] & 0xFF];
        D_00810170[0] = cam[0x28] - row[D_00810144[0] * 3];
        D_00810178[0] = cam[0x2A] - row[D_00810144[0] * 3 + 1];
        *(float *)0x700038A0 = D_00810170[0];
        *(float *)0x700038A4 = D_00810174[0];
        *(float *)0x700038A8 = D_00810178[0];
        *(float *)0x700038AC = 1.0f;
        func_001029C0(D_700036A0, D_00810144[0]);
        func_00102BB0(D_700036A0, D_700036A0, -row[D_00810144[0] * 3 + 2]);
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        D_00810170[0] = *(float *)0x700038A0;
        D_00810178[0] = *(float *)0x700038A8;
        half = D_00810154[0] / 2.0f;
        k = 0.10666667f * D_00810154[0];
        D_00810170[0] = D_00810170[0] + half * -(D_0081015C[0] / k);
        D_00810178[0] = D_00810178[0] + half * (D_00810158[0] / k);
        D_00810170[0] = D_00810170[0] * (0.10666667f * D_00810154[0]);
        D_00810178[0] = D_00810178[0] * (0.08533333f * D_00810154[0]);
      }
    }
    break;
  case 2:
  case 3:
  default:
    func_001AFF90(p);
    break;
  }
}
