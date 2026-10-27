// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.05% NEARMISS).
// Fixes read from the original instructions:
//  - Every state ends in `break`, not `return` (the original's dispatch has the
//    branch-likely + duplicated increment at the case-0 label that `break` gives);
//    the "idiom-13 delay-slot wall" verdict was wrong.
//  - func_001751A0 takes only the object (its definition in src/; no index argument).
//  - State 4/5 integrates self+0xC0 with a compound `+= self+0x26C` (fixes the FP
//    operand order of the add).

// State machine on self+7: state 0 starts clip 0xD4 and saves a lifted copy of the
// position (self+0x290); state 1 waits for self+0x200 bit 0x1000 and starts the
// func_001885B0 clip; state 2 runs func_001751A0/func_0016A8B0, advances on the pad
// mask (D_00810E74 & scratchpad 0x70003B76) or, while self+0x25C is 0, picks the next
// self+6 sub-state and restores the position; state 3 seeds speeds from D_00248630
// and starts clip 0xD5; states 4/5 decay speed, integrate self+0xC0 and settle to
// state 7 (self+5) when stopped; state 0x63 calls func_0021D2E0(0x78, 0).

extern void func_00102948(void *dst, void *src);
extern void func_0016A8B0(void *p);
extern void func_001749A0(void *self, int code, int a2, float a3);
extern void func_001751A0(void *p);
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
    break;
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
    break;

  case 2:
    func_001751A0(p);
    func_0016A8B0(p);
    if (D_00810E74 & *(unsigned short *)0x70003B76)
    {
      p[7] = p[7] + 1;
      break;
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
      break;
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
      break;
    }
    func_001751A0(p);
    func_0016A8B0(p);
    break;

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
    *(float *)(p + 0xC0) += *(float *)(p + 0x26C);
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
      break;
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
