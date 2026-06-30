// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-object update dispatch keyed on the state byte at +0xD: cases 1/2/3/0xC
// forward to the matching sub-updater (func_00189090 / func_00189330 /
// func_001899C0 / func_00189A20); case 0 tail-calls func_00188ED0() and returns
// early (skipping the default work). The default (any other state) publishes
// the qword at D_008103D0[0]+0x90 into self+0xD0 (copy_qw4) then runs
// func_001C9610(D_00275B40, self[0xC], self+0xD0).
//
// Match keys: ascending case labels (0,1,2,3,0xC) so mwcc emits the target's
// descending beq dispatch with the early-epilogue case-0; -sdatathreshold 4
// puts the 4-byte pointer D_00275B40 in sdata (gp-rel) while D_008103D0 stays
// absolute via an oversized array decl; the func_001C9610 call written exactly
// as the matched sibling func_001899C0 (proto int*, count inline as
// *(unsigned char *)(p+0xC)) to get the target's a1-before-a0 arg schedule.
extern void func_00188ED0(void);
extern void func_00189090(void *p);
extern void func_00189330(void *p);
extern void func_001899C0(void *p);
extern void func_00189A20(void *p);
extern void copy_qw4(void *dst, void *src);
extern void func_001C9610(int *base, int count, void *src);
extern int D_008103D0[2];
extern int *D_00275B40;

void func_00188DF0(unsigned char *p)
{
  unsigned char state;
  state = p[0xD];
  switch (state)
  {
    case 0:
      func_00188ED0();
      return;
    case 1:
      func_00189090(p);
      break;
    case 2:
      func_00189330(p);
      break;
    case 3:
      func_001899C0(p);
      break;
    case 0xC:
      func_00189A20(p);
      break;
    default:
      copy_qw4(p + 0xD0, (void *)(D_008103D0[0] + 0x90));
      func_001C9610(D_00275B40, *(unsigned char *)(p + 0xC), p + 0xD0);
      break;
  }
}
