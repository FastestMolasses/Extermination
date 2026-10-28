// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.78% NEARMISS).
// Fixes read from the original instructions:
// - The slot is taken as &D_00275670[idx] and its cursor read as slot[4]
//   (the spelling of the byte-matched twin func_001D6580), not as byte-offset
//   casts; with that, the global load and the slot address get separate registers.
// - The body pointer (cursor + 0x10) is formed BEFORE the cursor is advanced,
//   and the qword clear goes through it.
// - The DMAtag/VIF/GIFtag constants are written inline (no hoisted locals);
//   a 128-bit unsigned type clears the quadword.
//
// GIF/DMA packet builder. D_00275670 is an array of per-channel ring
// builders; element [idx] holds a write cursor at word [4]. Stamps the DMAtag
// at the cursor (byte +3 = 0x10, word +4 = 0, half +0 = 6 qwords), advances
// the cursor by 0x70, clears the next qword and writes the VIF word
// 0x50000005 at +0x1C, the GIFtag (+0x20) and its register list 0x4141 (+0x28),
// then copies the vectors a2 (4 words), a1 (3 words + 0), t0 (4 words) and
// a3 (3 words + 0) into +0x30..+0x6C. Returns the packet body pointer
// (cursor + 0x10).
typedef unsigned __int128 u128;
extern char **D_00275670;

char *func_001D63B0(int idx, int *a1, int *a2, int *a3, int *t0) {
  char **slot;
  char *pkt;
  char *body;

  slot = (char **)(&D_00275670[idx]);
  *((char *)(slot[4] + 3)) = 0x10;
  *((int *)(slot[4] + 4)) = 0;
  *((short *)(slot[4] + 0)) = 6;
  pkt = slot[4];
  body = pkt + 0x10;
  slot[4] = pkt + 0x70;
  *((u128 *)body) = 0;
  *((int *)(pkt + 0x1C)) = 0x50000005;
  *((long long *)(pkt + 0x20)) = 0x8001 | (((long long)0x4024C000) << 0x20);
  *((long long *)(pkt + 0x28)) = 0x4141;
  *(int *)(pkt + 0x30) = a2[0];
  *(int *)(pkt + 0x34) = a2[1];
  *(int *)(pkt + 0x38) = a2[2];
  *(int *)(pkt + 0x3C) = a2[3];
  *(int *)(pkt + 0x40) = a1[0];
  *(int *)(pkt + 0x44) = a1[1];
  *(int *)(pkt + 0x48) = a1[2];
  *(int *)(pkt + 0x4C) = 0;
  *(int *)(pkt + 0x50) = t0[0];
  *(int *)(pkt + 0x54) = t0[1];
  *(int *)(pkt + 0x58) = t0[2];
  *(int *)(pkt + 0x5C) = t0[3];
  *(int *)(pkt + 0x60) = a3[0];
  *(int *)(pkt + 0x64) = a3[1];
  *(int *)(pkt + 0x68) = a3[2];
  *(int *)(pkt + 0x6C) = 0;
  return body;
}
