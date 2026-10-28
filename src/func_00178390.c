// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Queries func_00199FA0 into two local vectors; on success places the actor:
// self+0x2E0 / +0x2E8 = the scratchpad point (0x700031B0 x / 0x700031B8 z)
// pushed 1.5 units along the x / z of the record whose pointer is at
// 0x700031D0 (+0x24 / +0x2C), self+0x2E4 = the second vector's y - 20.5,
// self+0x218 = self+0xC4; returns 1. Returns 0 otherwise. The scratchpad
// words are relocated externs (idiom-32).
extern int func_00199FA0(char *arg0, char *arg1);
extern float D_700031B0[4];
extern char *D_700031D0;

int func_00178390(char *self) {
    float lo[4];
    float hi[4];

    if (func_00199FA0((char *)lo, (char *)hi)) {
        *(float *)(self + 0x2E0) = D_700031B0[0] + 1.5f * *(float *)(D_700031D0 + 0x24);
        *(float *)(self + 0x2E8) = D_700031B0[2] + 1.5f * *(float *)(D_700031D0 + 0x2C);
        *(float *)(self + 0x2E4) = hi[1] - 20.5f;
        *(float *)(self + 0x218) = *(float *)(self + 0xC4);
        return 1;
    }
    return 0;
}
