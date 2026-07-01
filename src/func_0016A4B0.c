// NEARMISS func_0016A4B0  (vram 0x0016A4B0, 0x3FC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 94.05% mwcc233 (991202 83.79%). Fully recovered state-machine switch (all 7 states 0-5,0x63; all call args/prototypes verified against target incl. func_001749A0(self,code,a2,f) and func_0021D2E0(self,a1,a2)). Residual is a real idiom-13 delay-slot wall specific to this 5-way beq compare...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *dst, void *src);
extern void func_0016A8B0(void *p);
extern void func_001749A0(void *self, int code, int a2, float a3);
extern void func_001751A0(void *p, int idx);
extern int func_00175900(void *p, int a1);
extern void func_00178B90(void *p, int a1);
extern void func_00179880(void *p, void *a1);
extern void func_0017C580(void *p);
extern int func_00181950(void *p);
extern int func_00181A70(void *p);
extern int func_001885B0(void *p);
extern void func_0021D250(void *p, int a1);
extern void func_0021D2E0(void *p, int a1, int a2);
extern float D_00248630[];
extern unsigned short D_00810E74;

void func_0016A4B0(unsigned char *p)
{
  unsigned char st;

  st = p[7];
  switch (st) {
  case 0:
  {
    float k = 21.0f;
    p[7] = st + 1;
    func_001749A0(p, 0xD4, 0, 4.0f);
    func_00102948(p + 0x290, p + 0xB0);
    *(float *)(p + 0x294) = *(float *)(p + 0x294) + k;
    return;
  }

  case 1:
    if (*(int *)(p + 0x200) & 0x1000)
    {
      p[7] = st + 1;
      func_001749A0(p, func_001885B0(p), 0, 8.0f);
      *(unsigned short *)(p + 0x2E) = 0;
      *(float *)(p + 0x38) = 0.0f;
      *(short *)(p + 0x28) = 0;
      p[0x25C] = 0;
    }
    return;

  case 2:
    func_001751A0(p, st);
    func_0016A8B0(p);
    if (D_00810E74 & *(unsigned short *)0x70003B76)
    {
      p[7] = p[7] + 1;
      return;
    }
    if (p[0x25C] == 0)
    {
      if (p[5] == 0x10)
      {
        p[6] = 0;
      }
      else if (*(int *)(p + 0x24C) == 1)
      {
        p[6] = p[6] + 1;
      }
      else
      {
        p[6] = 0x28;
        p[0x1F0] = 0x22;
      }
      func_001749A0(p, func_001885B0(p), 0, 8.0f);
      *(float *)(p + 0xC0) = 0.0f;
      func_00102948(p + 0xB0, p + 0x290);
      *(float *)(p + 0xB4) = *(float *)(p + 0xB4) - 21.0f;
      return;
    }
    break;

  case 3:
    if ((*(unsigned short *)(p + 0x2E) == 3) && (*(short *)(p + 0x28) >= 0x18))
    {
      float t;
      p[7] = st + 1;
      t = D_00248630[p[0x25C]];
      *(float *)(p + 0x38) = t;
      *(float *)(p + 0x2E0) = -t / 120.0f;
      *(float *)(p + 0x26C) = -(*(float *)(p + 0xC0)) / 110.0f;
      *(int *)(p + 0x2EC) = 0x3ECCCCCD;
      func_001749A0(p, 0xD5, 0, 1.0f);
      *(float *)(p + 0x2F4) = *(float *)(p + 0xB4);
      return;
    }
    func_001751A0(p, st);
    func_0016A8B0(p);
    return;

  case 4:
    if (*(int *)(p + 0x200) & 0x1000)
    {
      p[7] = st + 1;
      func_001749A0(p, 0x72, 0, 8.0f);
    }
    /* fallthrough */
  case 5:
  {
    float zero = 0.0f;
    float t26c = *(float *)(p + 0x26C);
    float tc0 = *(float *)(p + 0xC0);
    *(float *)(p + 0xC0) = tc0 + t26c;
    if (*(float *)(p + 0x38) != zero)
    {
      float f = *(float *)(p + 0x38) + *(float *)(p + 0x2E0);
      *(float *)(p + 0x38) = f;
      if (f <= zero)
      {
        *(float *)(p + 0x38) = 0.0f;
        *(float *)(p + 0xC0) = 0.0f;
      }
      func_00178B90(p, 0);
    }
    if (func_00181950(p))
    {
      *(float *)(p + 0x38) = 0.0f;
    }
    if (func_00181A70(p))
    {
      *(float *)(p + 0xC0) = 0.0f;
      p[6] = 0x5B;
      return;
    }
    func_00179880(p, p + 0x2EC);
    if (func_00175900(p, 1))
    {
      func_0017C580(p);
    }
    else if ((*(float *)(p + 0x38) <= zero) && (*(float *)(p + 0xC0) == zero))
    {
      p[5] = 7;
      p[6] = 0;
      p[0x1F0] = 0xD;
    }
    if (p[0x23A] == 0x5D)
    {
      func_0021D250(p, 0);
    }
    break;
  }

  case 0x63:
    func_0021D2E0(p, 0x78, 0);
    break;
  }
}
