// NEARMISS func_00162DB0  (vram 0x00162DB0, 0x6E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// One pair in func_001B12B0(0.0f, +0xC0, 0.06981317f): the target sets $f14 before $f12 = 0; mwcc 2.3.3/2.4 set $f12 first. Literal, local, const and int-staged forms do not move it; the permuter matched only with the step set in case 0xA (uninitialised on the direct 0xB path), not used.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Player state +0x05 = 5, dispatched on the sub-state byte +0x06. Sub-state
// 0 is the fall start; 0xA / 0xB run until the footing update finds ground,
// then hand over to the landing func_0017C580.
//
//   0     four probes with func_00179450: at the position +0xB0 and at the
//         three D_00248580 points placed by the root matrix +0xD0 (scratch
//         D_700038A0); a probe counts when +0x258 is above
//         -(D_002488B0 - 1.8). Sub-state 0xA below sets the speed +0x38 =
//         D_00248560[k] and +0x2EC = D_00248570[k].
//         - Any probe counted: 0xA with k = +0x25C.
//         - Else, with a nonzero gait from func_00174AC0(self, 0): k = the
//           gait +0x23F; a gait-3 stick within 90 degrees of the facing
//           (func_001755B0 == 0) goes to 0xA, otherwise func_0017D080
//           nonzero gives sub-state 1 (+0x1F0 = 0xA, clip 0x83), else 0xA.
//         - Else k = +0x25C; +0x25C == 3 goes to 0xA, otherwise
//           func_0017D080 chooses between sub-state 1 and 0xA as above.
//         Every path then records the fall-start height +0x2F4 = +0xB4 and
//         sets +0x25F = 2.
//   1     waits for bit 0x8000 of +0x200 to clear.
//   2     at +0x3C <= 15: height +0xB4 = +0x294 - 0.8.
//   3     at +0x3C <= 12: sound 0xFF.
//   4     at the clip end (+0x200 & 0x1000): position from +0x290 / +0x298,
//         height +0x294 - 20.5, the clip func_00188550 picks, heading +0xC4
//         turned by pi, root matrix rebuilt; then sub-state 5 when
//         func_0017F320 is nonzero, else +0x05 = 9 / +0x06 = 0 / +0x1F0 =
//         0x10 / +0x0D = 0.
//   5     +0x2F4 = +0xB4, +0x05 = 7, +0x06 = 0, +0x1F0 = 0xD, +0x2EC =
//         -0.2f.
//   0xA   sub-state 0xB, +0x07 = 0, the deceleration +0x2E0 = speed / 60,
//         anim_clip_arbiter(self, 0x73, 8.0, n - 10) where n is clip 0x73's
//         frame count (func_001C61D0, kept in the scratchpad float
//         0x70003A20), func_00182870(self, 0); then the 0xB body.
//   0xB   +0xC0 eased toward 0 (func_001B12B0, at most 4 degrees a frame);
//         a speed above +0x2E0 loses +0x2E0 and func_00178B90 moves the
//         player, otherwise the speed is zeroed and func_001764E0 runs;
//         then func_00179880 and the footing update func_00175900. With the
//         footing flag +0x0A set, the landing func_0017C580 runs unless
//         func_00224290 returned nonzero. Without it, at the clip end
//         (+0x200 & 0x1000) with no speed left and func_00224290 zero:
//         +0x05 = 7, +0x06 = 0, +0x1F0 = 0xD, clip 0x72. A surface attr
//         +0x23A of 0x5D (the death floor) then calls func_0021D250(self,
//         0).
//   0x63  func_0021D2E0(self, 0x78, 0).
//
// Matching notes: hit starts at 0 before the switch and every probe ORs it;
// the cases end with break (MATCHING_GUIDE idiom-27); the 0xB speed test
// reads +0x38 / +0x2E0 after func_00224290 and subtracts with -=.
extern int func_00179450(void *p, void *m);
extern void func_001026A0(void *dst, void *src, void *m);
extern int func_00174AC0(void *p, int a1);
extern int func_001755B0(void *p);
extern int func_0017D080(void *p);
extern void func_001749A0(void *self, int code, int a2, float a3);
extern void func_001FBD50(void *self, int code, int a2, float a3);
extern int func_00188550(void *p);
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
extern float D_70003A20;

void func_00162DB0(unsigned char *self)
{
  unsigned char sub;
  int i;
  int hit;
  float *probe_point;

  sub = self[6];
  hit = 0;
  switch (sub) {
  case 0:
  {
    if (func_00179450(self, self + 0xB0) != 0 && !(*(float *)(self + 0x258) <= -(D_002488B0 - 1.8f))) {
      hit |= 1;
    }
    probe_point = D_00248580;
    for (i = 0; i < 3; i++, probe_point += 4) {
      func_001026A0(&D_700038A0, self + 0xD0, probe_point);
      if (func_00179450(self, &D_700038A0) != 0 && !(*(float *)(self + 0x258) <= -(D_002488B0 - 1.8f))) {
        hit |= 1;
      }
    }

    if (hit != 0) {
      self[6] = 0xA;
      *(float *)(self + 0x38) = D_00248560[self[0x25C]];
      *(float *)(self + 0x2EC) = D_00248570[self[0x25C]];
    } else if (func_00174AC0(self, 0) != 0) {
      if (self[0x23F] == 3 && func_001755B0(self) == 0) {
        self[6] = 0xA;
        *(float *)(self + 0x38) = D_00248560[self[0x23F]];
        *(float *)(self + 0x2EC) = D_00248570[self[0x23F]];
      } else if (func_0017D080(self) != 0) {
        self[6] = self[6] + 1;
        *(char *)(self + 0x1F0) = 0xA;
        func_001749A0(self, 0x83, 0, 4.0f);
      } else {
        self[6] = 0xA;
        *(float *)(self + 0x38) = D_00248560[self[0x23F]];
        *(float *)(self + 0x2EC) = D_00248570[self[0x23F]];
      }
    } else if (self[0x25C] == 3) {
      self[6] = 0xA;
      *(float *)(self + 0x38) = D_00248560[self[0x25C]];
      *(float *)(self + 0x2EC) = D_00248570[self[0x25C]];
    } else if (func_0017D080(self) != 0) {
      self[6] = self[6] + 1;
      *(char *)(self + 0x1F0) = 0xA;
      func_001749A0(self, 0x83, 0, 4.0f);
    } else {
      self[6] = 0xA;
      *(float *)(self + 0x38) = D_00248560[self[0x25C]];
      *(float *)(self + 0x2EC) = D_00248570[self[0x25C]];
    }
    *(float *)(self + 0x2F4) = *(float *)(self + 0xB4);
    self[0x25F] = 2;
    break;
  }

  case 1:
    if (!(*(int *)(self + 0x200) & 0x8000)) {
      self[6] = sub + 1;
    }
    break;

  case 2:
    if (*(float *)(self + 0x3C) <= 15.0f) {
      self[6] = sub + 1;
      *(float *)(self + 0xB4) = *(float *)(self + 0x294) - 0.8f;
    }
    break;

  case 3:
    if (*(float *)(self + 0x3C) <= 12.0f) {
      self[6] = sub + 1;
      func_001FBD50(self, 0xFF, 0, 300.0f);
    }
    break;

  case 4:
    if (*(int *)(self + 0x200) & 0x1000) {
      *(float *)(self + 0xB0) = *(float *)(self + 0x290);
      *(float *)(self + 0xB8) = *(float *)(self + 0x298);
      *(float *)(self + 0xB4) = *(float *)(self + 0x294) - 20.5f;
      func_001749A0(self, func_00188550(self), 0, 0.0f);
      *(float *)(self + 0xC4) = func_001B1470(3.1415927f + *(float *)(self + 0xC4));
      build_trs_matrix(self + 0xD0, self + 0xB0, self + 0xC0, self + 0x60);
      if (func_0017F320(self) != 0) {
        self[6] = 5;
        break;
      }
      self[5] = 9;
      self[6] = 0;
      *(char *)(self + 0x1F0) = 0x10;
      *(char *)(self + 0xD) = 0;
    }
    break;

  case 5:
    *(float *)(self + 0x2F4) = *(float *)(self + 0xB4);
    self[5] = 7;
    self[6] = 0;
    *(char *)(self + 0x1F0) = 0xD;
    *(int *)(self + 0x2EC) = 0xBE4CCCCD;
    break;

  case 0xA:
    self[6] = sub + 1;
    self[7] = 0;
    *(float *)(self + 0x2E0) = *(float *)(self + 0x38) / 60.0f;
    D_70003A20 = (float)func_001C61D0(*(int *)(self + 0x40), 0x73);
    anim_clip_arbiter(self, 0x73, 8.0f, D_70003A20 - 10.0f);
    func_00182870(self, 0);
    /* fallthrough */
  case 0xB:
  {
    int defer_landing;

    *(float *)(self + 0xC0) = func_001B12B0(0.0f, *(float *)(self + 0xC0), 0.06981317f);
    defer_landing = func_00224290(self);
    if (!(*(float *)(self + 0x38) <= *(float *)(self + 0x2E0))) {
      *(float *)(self + 0x38) -= *(float *)(self + 0x2E0);
      func_00178B90(self, 1);
    } else {
      *(int *)(self + 0x38) = 0;
      func_001764E0(self);
    }
    func_00179880(self, self + 0x2EC);
    func_00175900(self, 1);
    if (self[0xA] != 0) {
      if (defer_landing == 0) {
        func_0017C580(self);
      }
    } else if ((*(int *)(self + 0x200) & 0x1000) && *(float *)(self + 0x38) <= 0.0f && defer_landing == 0) {
      self[5] = 7;
      self[6] = 0;
      *(char *)(self + 0x1F0) = 0xD;
      func_001749A0(self, 0x72, 0, 8.0f);
    }
    if (self[0x23A] == 0x5D) {
      func_0021D250(self, 0);
      break;
    }
    break;
  }

  case 0x63:
    func_0021D2E0(self, 0x78, 0);
    break;
  }
}
