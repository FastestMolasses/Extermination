// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.38% NEARMISS).
// Fixes read from the original instructions:
//  - slot starts as the ring base and the counter-scaled offset is added with
//    `slot +=` (base materialised first, counter read early), with the table
//    entry computed between the two steps; this gives the original's
//    register colouring (slot, idx copy, e in the saved registers).
//  - arg1 is a pointer, matching func_001F2F90's definition (first param char *).

// Per-call ring-buffer slot writer. Computes two struct pointers:
//   slot = &D_007709C0[D_00275C40 * 0x90]  (0x90-stride ring entry, index in
//          gp-rel counter D_00275C40)
//   e    = &D_0025A350[idx * 0x60]         (0x60-stride table entry, by idx)
// then runs func_001F2F90(arg1, slot, e, idx) and func_001F3340(slot, e, idx),
// advances the ring counter D_00275C40 and wraps it back to 0 at 0x80.
extern void func_001F2F90(char *a, char *b, char *c, int d);
extern void func_001F3340(char *a, char *b, int c);
extern char D_0025A350[];
extern int D_00275C40;
extern char D_007709C0[];

void func_001F4010(int idx, char *arg1)
{
  char *slot = D_007709C0;
  char *e;
  slot += D_00275C40 * 0x90;
  e = &D_0025A350[idx * 0x60];
  func_001F2F90(arg1, slot, e, idx);
  func_001F3340(slot, e, idx);
  D_00275C40 += 1;
  if (D_00275C40 >= 0x80)
  {
    D_00275C40 = 0;
  }
}
