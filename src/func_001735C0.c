// NEARMISS func_001735C0  (vram 0x001735C0, 0x810 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 99.08% mwcc233 (991202 89.81% - idiom-13 nops + unsigned-char andi artifacts prove 233 is the lane). Body 100% recovered: full 9-state (0,1,2,3,0x50,0x51,0x52,0x63,0x64) nested switch state machine, all three inner switches, all call args/prototypes verified per-instruction, dispatch-con...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Boss/enemy attack-phase state machine on p[6] (states 0-3 intro/attack phases,
// 0x50/0x51/0x52 stagger+recover with countdown at p+0x28, 0x63 defeat branch,
// 0x64 death loop). Each attack phase runs an inner sub-state machine on p[7]:
// plays a clip from the 6-byte-stride short table D_00248690/92/94 (indexed by
// variant byte p[0x236]) via func_001749A0, waits on health thresholds from the
// 0x18/0xC-stride float tables D_002486A0..B4/D0/D4, spawns effect func_001FBD50
// (id 0x17D/0x17E/0x17F, 300.0f) storing its handle at p[0x302] (freed via
// func_0011A070 when the sub-object at *(p+0x18) signals done at +0xA), and
// latches pad input (D_00810E74 & *(u16*)0x70003B78) into p+0x2E to gate phase
// advance. Common tail every tick: func_001764E0, gravity p+0xB4 += -0.2f,
// func_00175900(p,1), func_001796C0.

extern int func_0011A070(int a0);
extern int func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00174AC0(unsigned char *e, int a1);
extern int func_00175900(unsigned char *e, int a1);
extern int func_001764E0(unsigned char *e);
extern int func_00178B90(unsigned char *e, int a1);
extern int func_001796C0(unsigned char *e);
extern int func_0017C440(unsigned char *e, int a1);
extern int func_0017C540(unsigned char *e);
extern int func_001FBD50(unsigned char *e, int idx, int a, float b);

extern short D_00248690[];
extern short D_00248692[];
extern short D_00248694[];
extern float D_002486A0[];
extern float D_002486A4[];
extern float D_002486A8[];
extern float D_002486AC[];
extern float D_002486B0[];
extern float D_002486B4[];
extern float D_002486D0[];
extern float D_002486D4[];
extern unsigned short D_00810E74;

void func_001735C0(unsigned char *p)
{
  unsigned char st, s, h;
  short t;

  st = p[6];
  switch (st) {
  case 0:
    p[6] = st + 1;
    p[7] = 0;
    *(int *)(p + 0x38) = 0;
    (*(unsigned char **)(p + 0x18))[0xA] = 0;
    p[0x302] = 0xFF;
    /* fallthrough */
  case 1:
    s = p[7];
    switch (s) {
    case 0:
      p[7] = s + 1;
      func_001749A0(p, D_00248690[p[0x236] * 3], 0, *(float *)(p + 0x1FC));
      *(unsigned short *)(p + 0x2E) = 0;
      break;
    case 1:
      if (!(*(int *)(p + 0x200) & 0x8000))
      {
        p[7] = s + 1;
      }
      break;
    case 2:
      if (*(float *)(p + 0x3C) <= D_002486A0[p[0x236] * 6])
      {
        p[7] = s + 1;
        (*(unsigned char **)(p + 0x18))[0] = 1;
        *(short *)(*(unsigned char **)(p + 0x18) + 0x36) = 3;
        p[0x302] = func_001FBD50(p, 0x17D, 0, 300.0f);
        p[0x25E] = 0x81;
      }
      if (D_00810E74 & *(unsigned short *)0x70003B78)
      {
        *(unsigned short *)(p + 0x2E) = 1;
      }
      break;
    case 3:
      if ((*(unsigned char **)(p + 0x18))[0xA])
      {
        p[6] = 0x50;
        h = p[0x302];
        if (h != -1)
        {
          func_0011A070(h);
          p[0x302] = 0xFF;
        }
      }
      else
      {
        if (D_00810E74 & *(unsigned short *)0x70003B78)
        {
          *(unsigned short *)(p + 0x2E) = 1;
        }
        if (*(float *)(p + 0x3C) <= D_002486A4[p[0x236] * 6])
        {
          (*(unsigned char **)(p + 0x18))[0] = 2;
        }
        if (*(float *)(p + 0x3C) <= D_002486D0[p[0x236] * 3])
        {
          if (*(unsigned short *)(p + 0x2E))
          {
            p[6] = p[6] + 1;
            p[7] = 0;
            func_001749A0(p, D_00248692[p[0x236] * 3], 0, 1.0f);
            (*(unsigned char **)(p + 0x18))[0] = 2;
          }
          else
          {
            p[7] = p[7] + 1;
          }
        }
      }
      break;
    case 4:
      if (*(int *)(p + 0x200) & 0x1000)
      {
        p[6] = 0x63;
      }
      break;
    }
    break;
  case 2:
    s = p[7];
    switch (s) {
    case 0:
      p[7] = s + 1;
      *(unsigned short *)(p + 0x2E) = 0;
      break;
    case 1:
      if (*(float *)(p + 0x3C) <= D_002486A8[p[0x236] * 6])
      {
        p[7] = s + 1;
        (*(unsigned char **)(p + 0x18))[0] = 1;
        *(short *)(*(unsigned char **)(p + 0x18) + 0x36) = 3;
        p[0x302] = func_001FBD50(p, 0x17E, 0, 300.0f);
        p[0x25E] = 0x82;
      }
      if (D_00810E74 & *(unsigned short *)0x70003B78)
      {
        *(unsigned short *)(p + 0x2E) = 1;
      }
      break;
    case 2:
      if ((*(unsigned char **)(p + 0x18))[0xA])
      {
        p[6] = 0x50;
        h = p[0x302];
        if (h != -1)
        {
          func_0011A070(h);
          p[0x302] = 0xFF;
        }
      }
      else
      {
        if (D_00810E74 & *(unsigned short *)0x70003B78)
        {
          *(unsigned short *)(p + 0x2E) = 1;
        }
        if (*(float *)(p + 0x3C) <= D_002486D4[p[0x236] * 3])
        {
          if (*(unsigned short *)(p + 0x2E))
          {
            p[6] = p[6] + 1;
            p[7] = 0;
            func_001749A0(p, D_00248694[p[0x236] * 3], 0, 1.0f);
            (*(unsigned char **)(p + 0x18))[0] = 2;
          }
          else
          {
            p[7] = p[7] + 1;
          }
        }
      }
      break;
    case 3:
      if (*(int *)(p + 0x200) & 0x1000)
      {
        p[6] = 0x63;
      }
      else if (*(float *)(p + 0x3C) <= D_002486AC[p[0x236] * 6])
      {
        (*(unsigned char **)(p + 0x18))[0] = 2;
      }
      break;
    }
    break;
  case 3:
    s = p[7];
    switch (s) {
    case 0:
      p[7] = s + 1;
      break;
    case 1:
      if (*(float *)(p + 0x3C) <= D_002486B0[p[0x236] * 6])
      {
        p[7] = s + 1;
        (*(unsigned char **)(p + 0x18))[0] = 1;
        *(short *)(*(unsigned char **)(p + 0x18) + 0x36) = 5;
        p[0x302] = func_001FBD50(p, 0x17F, 0, 300.0f);
        p[0x25E] = 0x82;
      }
      break;
    case 2:
      if ((*(unsigned char **)(p + 0x18))[0xA])
      {
        p[6] = 0x50;
        h = p[0x302];
        if (h != -1)
        {
          func_0011A070(h);
          p[0x302] = 0xFF;
        }
      }
      else if (*(float *)(p + 0x3C) <= D_002486B4[p[0x236] * 6])
      {
        p[7] = s + 1;
        (*(unsigned char **)(p + 0x18))[0] = 2;
      }
      break;
    case 3:
      if (*(int *)(p + 0x200) & 0x1000)
      {
        p[6] = 0x63;
      }
      break;
    }
    break;
  case 0x50:
    p[6] = st + 1;
    *(short *)(p + 0x28) = 4;
    /* fallthrough */
  case 0x51:
    t = *(short *)(p + 0x28);
    *(short *)(p + 0x28) = t - 1;
    if (t == 0)
    {
      p[6] = p[6] + 1;
      if (p[0x236] == 0)
      {
        func_001749A0(p, 0x10F, 0, 4.0f);
      }
      else
      {
        func_001749A0(p, 0x1C1, 0, 4.0f);
      }
    }
    else
    {
      *(int *)(p + 0x204) = 0;
    }
    break;
  case 0x52:
    if (*(int *)(p + 0x200) & 0x1000)
    {
      p[6] = 0x63;
    }
    break;
  case 0x63:
    func_00174AC0(p, 1);
    if (p[0x23F] > 1)
    {
      p[6] = p[6] + 1;
      func_0017C440(p, 0);
    }
    else
    {
      p[0x25C] = 0;
      func_0017C540(p);
    }
    break;
  case 0x64:
    func_00174AC0(p, 1);
    func_00178B90(p, 0);
    if (!(*(int *)(p + 0x200) & 0x8000))
    {
      func_0017C540(p);
    }
    break;
  }
  func_001764E0(p);
  *(float *)(p + 0xB4) += -0.2f;
  func_00175900(p, 1);
  func_001796C0(p);
}
