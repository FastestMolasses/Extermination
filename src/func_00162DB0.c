// NEARMISS func_00162DB0  (vram 0x00162DB0, 0x6E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 91.18% mwcc233 (82.29% 991202). Body/structure fully recovered, all state-machine cases correct. Two residual classes: (1) register-allocation-ORDER permutation in the case-0 probe loop (s0/s1 swapped for counter vs hit-flag, similar to characterized func_00175900 sibling wall); (2) an F...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Animation-clip state machine (self+6 state byte), sibling of func_001634A0/
// func_0016A4B0. state 0: probes func_00179450 on self+0xB0 and on each of 3
// entries of the D_00248580 table (transformed via func_001026A0 into scratch
// D_700038A0), ORing a "hit" flag from a self+0x258 threshold test; if any hit,
// jumps to a shared clip-pick block keyed by self+0x25C. Else checks
// func_00174AC0(self,0): if aiming, picks clip by self+0x23F (falling back to
// func_0017D080 idle-clip choose when func_001755B0 fails); otherwise picks by
// self+0x25C directly (falling back to func_0017D080 similarly). state 1-5:
// short thresholds transitioning to speed/clip setup; state 0xA/0xB integrate
// self+0x38 down by self+0x2E0, exit to func_0021D250 when self+0x23A==0x5D.
// state 0x63: terminal -- func_0021D2E0(self, 0x78, 0).
extern int func_00179450(void *p, void *m);
extern void func_001026A0(void *dst, void *src, void *m);
extern int func_00174AC0(void *p, int a1);
extern int func_001755B0(void *p);
extern int func_0017D080(void *p);
extern void func_001749A0(void *self, short code, int a2, float a3);
extern void func_001FBD50(void *self, int code, int a2, float a3);
extern short func_00188550(void *p);
extern float func_001B1470(float);
extern float func_001B12B0(float a, float b, float c);
extern int func_001C61D0(int idx, int clip);
extern void anim_clip_arbiter(void *p, int clip, float speed, float f);
extern void build_trs_matrix(void *dst, void *pos, void *rot, void *scl);
extern int func_0017F320(void *p);
extern int func_00224290(void *p);
extern void func_00178B90(void *p, int a1);
extern void func_001764E0(void *p);
extern void func_00179880(void *p, void *a1);
extern void func_00175900(void *p, int a1);
extern void func_0017C580(void *p);
extern void func_00182870(void *p, int a1);
extern void func_0021D250(void *p, int a1);
extern void func_0021D2E0(void *p, int a1, int a2);

extern float D_00248560[];
extern float D_00248570[];
extern float D_00248580[];
extern float D_002488B0;
extern float D_700038A0;

void func_00162DB0(unsigned char *arg0)
{
  unsigned char st;

  st = arg0[6];
  switch (st) {
  case 0:
  {
    int i;
    float *walk;
    int hit;

    hit = 0;
    if (func_00179450(arg0, arg0 + 0xB0) != 0 && !(*(float *)(arg0 + 0x258) <= -(D_002488B0 - 1.8f))) {
      hit = 1;
    }
    walk = D_00248580;
    for (i = 0; i < 3; i++, walk += 4) {
      func_001026A0(&D_700038A0, arg0 + 0xD0, walk);
      if (func_00179450(arg0, &D_700038A0) != 0 && !(*(float *)(arg0 + 0x258) <= -(D_002488B0 - 1.8f))) {
        hit |= 1;
      }
    }

    if (hit != 0) {
      arg0[6] = 0xA;
      *(float *)(arg0 + 0x38) = D_00248560[arg0[0x25C]];
      *(int *)(arg0 + 0x2EC) = *(int *)&D_00248570[arg0[0x25C]];
    } else if (func_00174AC0(arg0, 0) != 0) {
      if (arg0[0x23F] == 3 && func_001755B0(arg0) == 0) {
        arg0[6] = 0xA;
        *(float *)(arg0 + 0x38) = D_00248560[arg0[0x23F]];
        *(int *)(arg0 + 0x2EC) = *(int *)&D_00248570[arg0[0x23F]];
      } else if (func_0017D080(arg0) != 0) {
        arg0[6] = arg0[6] + 1;
        *(char *)(arg0 + 0x1F0) = 0xA;
        func_001749A0(arg0, 0x83, 0, 4.0f);
        return;
      } else {
        arg0[6] = 0xA;
        *(float *)(arg0 + 0x38) = D_00248560[arg0[0x23F]];
        *(int *)(arg0 + 0x2EC) = *(int *)&D_00248570[arg0[0x23F]];
      }
    } else if (arg0[0x25C] == 3) {
      arg0[6] = 0xA;
      *(float *)(arg0 + 0x38) = D_00248560[arg0[0x25C]];
      *(int *)(arg0 + 0x2EC) = *(int *)&D_00248570[arg0[0x25C]];
    } else if (func_0017D080(arg0) != 0) {
      arg0[6] = arg0[6] + 1;
      *(char *)(arg0 + 0x1F0) = 0xA;
      func_001749A0(arg0, 0x83, 0, 4.0f);
      return;
    } else {
      arg0[6] = 0xA;
      *(float *)(arg0 + 0x38) = D_00248560[arg0[0x25C]];
      *(int *)(arg0 + 0x2EC) = *(int *)&D_00248570[arg0[0x25C]];
    }
    *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
    arg0[0x25F] = 2;
    return;
  }

  case 1:
    if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
      arg0[6] = st + 1;
    }
    return;

  case 2:
    if (*(float *)(arg0 + 0x3C) <= 15.0f) {
      arg0[6] = st + 1;
      *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x294) - 0.8f;
    }
    return;

  case 3:
    if (*(float *)(arg0 + 0x3C) <= 12.0f) {
      arg0[6] = st + 1;
      func_001FBD50(arg0, 0xFF, 0, 300.0f);
    }
    return;

  case 4:
    if (*(int *)(arg0 + 0x200) & 0x1000) {
      *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0x290);
      *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0x298);
      *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0x294) - 20.5f;
      func_001749A0(arg0, func_00188550(arg0), 0, 0.0f);
      *(float *)(arg0 + 0xC4) = func_001B1470(3.1415927f + *(float *)(arg0 + 0xC4));
      build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
      if (func_0017F320(arg0) != 0) {
        arg0[6] = 5;
        return;
      }
      arg0[5] = 9;
      arg0[6] = 0;
      *(char *)(arg0 + 0x1F0) = 0x10;
      *(char *)(arg0 + 0xD) = 0;
    }
    return;

  case 5:
    *(float *)(arg0 + 0x2F4) = *(float *)(arg0 + 0xB4);
    arg0[5] = 7;
    arg0[6] = 0;
    *(char *)(arg0 + 0x1F0) = 0xD;
    *(int *)(arg0 + 0x2EC) = 0xBE4CCCCD;
    return;

  case 0xA:
    arg0[6] = st + 1;
    arg0[7] = 0;
    *(float *)(arg0 + 0x2E0) = *(float *)(arg0 + 0x38) / 60.0f;
    *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(arg0 + 0x40), 0x73);
    anim_clip_arbiter(arg0, 0x73, 8.0f, *(float *)0x70003A20 - 10.0f);
    func_00182870(arg0, 0);
    /* fallthrough */
  case 0xB:
  {
    float t38, t2e0;
    int idle;

    *(float *)(arg0 + 0xC0) = func_001B12B0(0.0f, *(float *)(arg0 + 0xC0), 0.06981317f);
    t38 = *(float *)(arg0 + 0x38);
    t2e0 = *(float *)(arg0 + 0x2E0);
    idle = func_00224290(arg0);
    if (!(t38 <= t2e0)) {
      *(float *)(arg0 + 0x38) = t38 - t2e0;
      func_00178B90(arg0, 1);
    } else {
      *(float *)(arg0 + 0x38) = 0.0f;
      func_001764E0(arg0);
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
    if (arg0[0xA] != 0) {
      if (idle == 0) {
        func_0017C580(arg0);
      }
    } else if ((*(int *)(arg0 + 0x200) & 0x1000) && *(float *)(arg0 + 0x38) <= 0.0f && idle == 0) {
      arg0[5] = 7;
      arg0[6] = 0;
      *(char *)(arg0 + 0x1F0) = 0xD;
      func_001749A0(arg0, 0x72, 0, 8.0f);
    }
    if (arg0[0x23A] == 0x5D) {
      func_0021D250(arg0, 0);
      return;
    }
    break;
  }

  case 0x63:
    func_0021D2E0(arg0, 0x78, 0);
    break;
  }
}
