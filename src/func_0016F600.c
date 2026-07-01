// NEARMISS func_0016F600  (vram 0x0016F600, 0x6EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 79.81% mwcc233 (76.40% 991202). Logic and control flow fully recovered (validated against sibling func_00171320's identical anim_matrix_dispatch/D_00275B40 tail template and cross-checked signatures for func_001749A0/func_001FBD50/func_0016F5D0). Residual is a mix of: (1) register-alloca...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Melee/impact wind-up state machine (mode byte at self+7), sibling of
// func_00171320 (shares the exact anim_matrix_dispatch + D_00275B40 tail
// block). case 0: advance mode, seed self+0x2E0/0x2E4 from self+0x27C/0x278,
// arm an 8-tick blend counter self+0x28, compute per-tick deltas
// self+0x26C/0x270 toward 0.5, and stash a facing angle at self+0x218 via
// func_0011E620/func_001B1470 off the linked-entity transform at self+0x20.
// case 1: ticks the blend counter; on expiry (0), snaps self+0x27C/0x278 to
// 0.5, fires a pair of func_001749A0 clip blends picked from
// D_00248B88/98 (when self+5 is 0x1D/0x1E) or D_00248C68/78 (otherwise),
// indexed by self+0x275, then plays func_001FBD50(self,0x163,0,300.0f);
// otherwise integrates self+0x27C/0x278 by the per-tick deltas. Either way
// falls into the shared tail (matrix-pick). case 2: if self+0x200 bit
// 0x1000 is set and either input-mask half at D_00810E70 hits, advances
// mode and repeats the clip-blend pick (into D_00248B88/C68 depending on
// self+5) at 300.0f volume with fresh 8-tick deltas; otherwise commits a
// terminal state (self+6=0x65) via a small self+5 -> self+0x1F0 lookup
// (0x1D->0x31, 0x1E->0x32, 0x1F->0x34, 0x20->0x35) and resets via
// func_0016F5D0. case 3: ticks a second 8-tick counter; on expiry alternates
// self+5 between its "left"/"right" pair (0x1D<->0x1E or 0x1F<->0x20) based
// on the D_00810E70 input-mask half, mirrors self+0x1F0 to match, resets
// mode/self+6, and reseeds self+0x27C/0x278 from self+0x2E0/0x2E4;
// otherwise integrates self+0x27C by its delta and self+0x278 by the OTHER
// delta (asymmetric vs case 1). Then falls into the SAME shared tail.
// Shared tail (also entered directly for case 2's early-return and again
// unconditionally at the end): unless self+0x1F0==0x33 (bare
// anim_matrix_dispatch), calls anim_matrix_dispatch then copies either the
// bone-transform block (self+0x1F0 in {0x32,0x35}, or self+0x275==4, or
// self+0x2F2 set) via copy_qw4 from *(D_00275B40+0x10)+0x90 into self+0x2A0,
// or (fallback) just the translate (*(D_00275B40+0x10)+0xC0/C4/C8) into
// self+0x2D0/2D4/2D8. Finally, whenever self+0x1F0==0x33 at the very end,
// runs anim_eval_skeleton (unless the tail already ran this tick) and
// refreshes self+0x2D0/2D4/2D8 from the same base translate.
extern void anim_eval_skeleton(void *p);
extern void anim_matrix_dispatch(void *p);
extern void copy_qw4(void *dst, void *src);
extern float func_0011E620(unsigned char st, float a, float b);
extern void func_0016F5D0(void *p);
extern void func_001749A0(void *self, short code, int a2, float a3);
extern float func_001B1470(float a);
extern void func_001FBD50(void *self, short code, int a2, float a3);

extern short D_00248680[];
extern short D_00248B88[];
extern short D_00248B98[];
extern short D_00248C68[];
extern short D_00248C78[];
extern int D_00275B40;
extern unsigned short D_00810E70;

void func_0016F600(unsigned char *arg0)
{
  unsigned char st;
  int done;

  st = arg0[7];
  done = 0;

  switch (st) {
  case 0:
  {
    void *link;
    float ang;

    arg0[7] = st + 1;
    *(float *)(arg0 + 0x2E0) = *(float *)(arg0 + 0x27C);
    *(float *)(arg0 + 0x2E4) = *(float *)(arg0 + 0x278);
    *(short *)(arg0 + 0x28) = 8;
    *(float *)(arg0 + 0x26C) = (0.5f - *(float *)(arg0 + 0x27C)) / 8.0f;
    *(float *)(arg0 + 0x270) = (0.5f - *(float *)(arg0 + 0x278)) / 8.0f;
    link = *(void **)(arg0 + 0x20);
    *(float *)0x70003A20 = func_0011E620(st, -(*(float *)((char *)link + 0xC8)), *(float *)((char *)link + 0xC0));
    ang = *(float *)0x70003A20;
    *(float *)(arg0 + 0x218) = func_001B1470(1.5707964f + ang);
    /* fallthrough */
  }
  case 1:
  {
    short cnt = *(short *)(arg0 + 0x28);
    *(short *)(arg0 + 0x28) = cnt - 1;
    if (cnt == 0) {
      unsigned char st5;

      arg0[7] = arg0[7] + 1;
      *(float *)(arg0 + 0x27C) = 0.5f;
      *(float *)(arg0 + 0x278) = 0.5f;
      st5 = arg0[5];
      if (st5 == 0x1D || st5 == 0x1E) {
        func_001749A0(arg0, D_00248B88[arg0[0x275]], 0, 0.0f);
        func_001749A0(arg0, D_00248B98[arg0[0x275]], 0, 1.0f);
      } else {
        func_001749A0(arg0, D_00248C68[arg0[0x275]], 0, 0.0f);
        func_001749A0(arg0, D_00248C78[arg0[0x275]], 0, 1.0f);
      }
      func_001FBD50(arg0, 0x163, 0, 300.0f);
    } else {
      *(float *)(arg0 + 0x27C) = *(float *)(arg0 + 0x27C) + *(float *)(arg0 + 0x26C);
      *(float *)(arg0 + 0x278) = *(float *)(arg0 + 0x278) + *(float *)(arg0 + 0x270);
    }
    if (arg0[0x1F0] == 0x33) {
      anim_matrix_dispatch(arg0);
    } else {
      int hit;

      anim_matrix_dispatch(arg0);
      hit = 0;
      if (arg0[0x1F0] == 0x32 || arg0[0x1F0] == 0x35) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      } else if (arg0[0x275] == 4) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      } else if (arg0[0x2F2] != 0) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      }
      done = 1;
      if (hit == 0) {
        *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
        *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
        *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
      }
    }
    break;
  }

  case 2:
    if (*(int *)(arg0 + 0x200) & 0x1000) {
      if ((D_00810E70 & *(unsigned short *)0x70003B7C) || (D_00810E70 & *(unsigned short *)0x70003B7E)) {
        unsigned char st5;

        arg0[7] = arg0[7] + 1;
        func_001FBD50(arg0, D_00248680[arg0[0x275]], 0, 300.0f);
        *(short *)(arg0 + 0x28) = 8;
        *(float *)(arg0 + 0x26C) = (*(float *)(arg0 + 0x2E0) - 0.5f) / 8.0f;
        *(float *)(arg0 + 0x270) = (*(float *)(arg0 + 0x2E4) - 0.5f) / 8.0f;
        st5 = arg0[5];
        if (st5 == 0x1D || st5 == 0x1E) {
          func_001749A0(arg0, D_00248B88[arg0[0x275]], 0, 0.0f);
        } else {
          func_001749A0(arg0, D_00248C68[arg0[0x275]], 0, 0.0f);
        }
      } else {
        unsigned char st5;
        unsigned char nst;

        arg0[6] = 0x65;
        st5 = arg0[5];
        switch (st5) {
        case 0x1D: nst = 0x31; break;
        case 0x1E: nst = 0x32; break;
        case 0x1F: nst = 0x34; break;
        case 0x20: nst = 0x35; break;
        default: nst = arg0[0x1F0]; break;
        }
        arg0[0x1F0] = nst;
        func_0016F5D0(arg0);
      }
    }
    break;

  case 3:
  {
    short cnt = *(short *)(arg0 + 0x28);
    float f278;

    *(short *)(arg0 + 0x28) = cnt - 1;
    if (cnt == 0) {
      unsigned char st5;
      unsigned char nst;

      st5 = arg0[5];
      if (st5 == 0x1D || st5 == 0x1E) {
        if (D_00810E70 & *(unsigned short *)0x70003B7E) {
          arg0[5] = 0x1E;
          nst = 0x32;
        } else {
          arg0[5] = 0x1D;
          nst = 0x31;
        }
      } else if (D_00810E70 & *(unsigned short *)0x70003B7E) {
        arg0[5] = 0x20;
        nst = 0x35;
      } else {
        arg0[5] = 0x1F;
        nst = 0x34;
      }
      arg0[0x1F0] = nst;
      arg0[6] = 2;
      arg0[7] = 0;
      *(float *)(arg0 + 0x27C) = *(float *)(arg0 + 0x2E0);
      f278 = *(float *)(arg0 + 0x2E4);
    } else {
      *(float *)(arg0 + 0x27C) = *(float *)(arg0 + 0x27C) + *(float *)(arg0 + 0x26C);
      f278 = *(float *)(arg0 + 0x278) + *(float *)(arg0 + 0x270);
    }
    *(float *)(arg0 + 0x278) = f278;

    if (arg0[0x1F0] == 0x33) {
      anim_matrix_dispatch(arg0);
    } else {
      int hit;

      anim_matrix_dispatch(arg0);
      hit = 0;
      if (arg0[0x1F0] == 0x32 || arg0[0x1F0] == 0x35) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      } else if (arg0[0x275] == 4) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      } else if (arg0[0x2F2] != 0) {
        copy_qw4(arg0 + 0x2A0, (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        hit = 1;
      }
      done = 1;
      if (hit == 0) {
        *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
        *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
        *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
      }
    }
    break;
  }
  }

  if (arg0[0x1F0] == 0x33) {
    if (done == 0) {
      anim_eval_skeleton(arg0);
    }
    *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
    *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
    *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
  }
}
