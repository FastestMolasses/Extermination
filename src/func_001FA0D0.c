// NEARMISS func_001FA0D0  (vram 0x001FA0D0, 0x260 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring / store-scheduling permuter class. Logic fully recovered. Two artifacts only: (1) phase-0 writes D_00282157/D_00282158 in the opposite store order vs target (mwcc schedules the direct store ahead of the read-modify-write); (2) phase-2's index bump+sign-extend (dsll32/dsra32) is ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (best: 92.9% mwcc 2.3.3, 70.2% mwcc 991202). Logic fully recovered.
// A 3-phase sequencer over a global array (base D_00281FD0, stride 0x60; the byte at
// element-offset +3 is D_00281FD3, and int fields at +0x30/+0x34/+0x38 are
// D_00282000/04/08). D_00282157 is the phase, D_00282158 the current element index.
// Phase 0 (scan): from index D_00282158, walk elements (idx < 3) until one whose
// byte+3 == 1; on a hit advance the phase and record the index; if no hit and the
// phase is still 0, reset the index to 0.
// Phase 1 (begin): if the active element's byte != 1, abort back to phase 0 and bump
// the index (wrapping at 3); else if func_00113280(1)==2, zero a 3-byte scratch and
// call func_00112610(field0x30, field0x34, field0x38, &scratch); on success advance
// the phase.
// Phase 2 (finish): if the element byte != 1, run func_00113478(1), reset to phase 0,
// bump+wrap the index; else once func_00112D18(1)==0, reset phase 0, mark the element
// byte = 2, bump+wrap the index.
//
// Residual wall (NOT the clean-store nop): two artifacts only -- (1) phase-0 writes
// D_00282157/D_00282158 in the opposite store order vs the target (mwcc schedules the
// direct store ahead of the read-modify-write); (2) phase-2's index bump+sign-extend
// is colored into a0 where the target keeps it in the callee-saved s0 it already holds
// idx in. Register-coloring / store-scheduling permuter class.
extern int func_00112610(int a, int b, int c, char *out);
extern int func_00112D18(int a);
extern int func_00113280(int a);
extern void func_00113478(int a);
extern char D_00282000;
extern char D_00282004;
extern char D_00282008;
extern char D_00281FD0;
extern char D_00281FD3;
extern signed char D_00282157;
extern signed char D_00282158;

void func_001FA0D0(void) {
    signed char idx;
    int off;
    char *p;
    char buf[3];

    switch (D_00282157) {
    case 0:
        idx = D_00282158;
        p = &D_00281FD0 + idx * 0x60;
        while (idx < 3) {
            if (*(signed char *)(p + 3) == 1) {
                D_00282157 += 1;
                D_00282158 = idx;
                break;
            }
            p += 0x60;
            idx = idx + 1;
        }
        if (D_00282157 == 0) {
            D_00282158 = 0;
        }
        break;
    case 1:
        off = D_00282158 * 0x60;
        if (*(signed char *)(&D_00281FD3 + off) != 1) {
            idx = D_00282158 + 1;
            D_00282157 = 0;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282158 = idx;
            break;
        }
        if (func_00113280(1) == 2) {
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
            if (func_00112610(*(int *)(&D_00282000 + off),
                              *(int *)(&D_00282004 + off),
                              *(int *)(&D_00282008 + off), buf) != 0) {
                D_00282157 += 1;
            }
        }
        break;
    case 2:
        idx = D_00282158;
        p = &D_00281FD3 + idx * 0x60;
        if (*(signed char *)p != 1) {
            func_00113478(1);
            D_00282157 = 0;
            idx = idx + 1;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282158 = idx;
            break;
        }
        if (func_00112D18(1) == 0) {
            D_00282157 = 0;
            *(signed char *)p = 2;
            idx = idx + 1;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282158 = idx;
        }
        break;
    }
}
