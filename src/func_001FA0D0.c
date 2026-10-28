// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.89% NEARMISS).
// Fixes read from the original instructions:
//  - The phase (0x00282157) and the index (0x00282158) are two members of one
//    struct, not two separate externs: with one object mwcc keeps the source
//    order of the phase increment and the index store in phase 0's hit path.
//  - Phase 2's index bump is a compound assignment (`idx += 1`), which keeps the
//    bumped index in the saved register that already holds idx.
//
// A 3-phase sequencer over a global array (base D_00281FD0, stride 0x60; the byte at
// element-offset +3 is D_00281FD3, and int fields at +0x30/+0x34/+0x38 are
// D_00282000/04/08). D_00282157.phase is the phase, D_00282157.idx the current
// element index.
// Phase 0 (scan): from the current index, walk elements (idx < 3) until one whose
// byte+3 == 1; on a hit advance the phase and record the index; if no hit and the
// phase is still 0, reset the index to 0.
// Phase 1 (begin): if the active element's byte != 1, abort back to phase 0 and bump
// the index (wrapping at 3); else if func_00113280(1)==2, zero a 3-byte scratch and
// call func_00112610(field0x30, field0x34, field0x38, &scratch); on success advance
// the phase.
// Phase 2 (finish): if the element byte != 1, run func_00113478(1), reset to phase 0,
// bump+wrap the index; else once func_00112D18(1)==0, reset phase 0, mark the element
// byte = 2, bump+wrap the index.
extern int func_00112610(int a, int b, int c, char *out);
extern int func_00112D18(int a);
extern int func_00113280(int a);
extern void func_00113478(int a);
extern char D_00282000;
extern char D_00282004;
extern char D_00282008;
extern char D_00281FD0;
extern char D_00281FD3;
/* The phase byte and the element-index byte are adjacent members of one object. */
typedef struct {
    signed char phase; /* 0x00282157 */
    signed char idx;   /* 0x00282158 */
} Seq;
extern Seq D_00282157;

void func_001FA0D0(void) {
    signed char idx;
    int off;
    char *p;
    char buf[3];

    switch (D_00282157.phase) {
    case 0:
        idx = D_00282157.idx;
        p = &D_00281FD0 + idx * 0x60;
        while (idx < 3) {
            if (*(signed char *)(p + 3) == 1) {
                D_00282157.phase += 1;
                D_00282157.idx = idx;
                break;
            }
            p += 0x60;
            idx = idx + 1;
        }
        if (D_00282157.phase == 0) {
            D_00282157.idx = 0;
        }
        break;
    case 1:
        off = D_00282157.idx * 0x60;
        if (*(signed char *)(&D_00281FD3 + off) != 1) {
            idx = D_00282157.idx + 1;
            D_00282157.phase = 0;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282157.idx = idx;
            break;
        }
        if (func_00113280(1) == 2) {
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
            if (func_00112610(*(int *)(&D_00282000 + off),
                              *(int *)(&D_00282004 + off),
                              *(int *)(&D_00282008 + off), buf) != 0) {
                D_00282157.phase += 1;
            }
        }
        break;
    case 2:
        idx = D_00282157.idx;
        p = &D_00281FD3 + idx * 0x60;
        if (*(signed char *)p != 1) {
            func_00113478(1);
            D_00282157.phase = 0;
            idx += 1;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282157.idx = idx;
            break;
        }
        if (func_00112D18(1) == 0) {
            D_00282157.phase = 0;
            *(signed char *)p = 2;
            idx += 1;
            if (idx >= 3) {
                idx = 0;
            }
            D_00282157.idx = idx;
        }
        break;
    }
}
