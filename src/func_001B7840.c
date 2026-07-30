// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: door/cutscene-script opcode handler (see docs/FINDINGS.md
// "DOOR SCRIPTS DECODED"), signature (actor, blk, rec). Sub-opcode is
// rec[+0x8]; a 10-entry jump table selects one of ten fade/sound/wait
// actions. Return 1 = step complete (advance the script), 0 = still busy.
//   rec[+0x8]  = sub-opcode (0..9)
//   rec[+0x14] = handle passed to the fade/sound helpers
//   rec[+0xC]  = wait duration in frames (float)
//   rec[+0x10] = wait timer in frames (float), reset to 0 on entry
//   blk[+0x4]  = per-step state byte (0 = first frame, 1 = counting)
//   D_0028A9A0 = global fade/transition state (short)
// Sub-opcodes 0/1/2 are the "off/0" variants of 6/7/8 ("on/1"); 3 and 9 are
// the same timed-wait step with helper argument 0 vs 1.

extern short D_0028A9A0;

extern void func_001AED80(int mode);
extern void func_001AEDB0(int mode);
extern void func_001AEDE0(int handle, int mode);
extern void func_001AEE10(int handle, int mode);

int func_001B7840(unsigned char *actor, unsigned char *blk, unsigned char *rec)
{
    switch (*(int *)(rec + 0x8)) {
    case 0:
        func_001AEE10(*(int *)(rec + 0x14), 0);
        break;
    case 1:
        func_001AEDE0(*(int *)(rec + 0x14), 0);
        break;
    case 2:
        func_001AED80(0);
        break;
    case 3:
        switch (blk[4]) {
        case 0:
            func_001AEDB0(0);
            blk[4] = blk[4] + 1;
            *(float *)(rec + 0x10) = 0.0f;
            return 0;
        case 1:
            if (*(float *)(rec + 0x10) < *(float *)(rec + 0xC)) {
                *(float *)(rec + 0x10) += 1.0f;
                return 0;
            }
            break;
        }
        break;
    case 4:
        if (D_0028A9A0 != 0) {
            return 0;
        }
        break;
    case 5:
        if (D_0028A9A0 != 2) {
            return 0;
        }
        break;
    case 6:
        func_001AEE10(*(int *)(rec + 0x14), 1);
        break;
    case 7:
        func_001AEDE0(*(int *)(rec + 0x14), 1);
        break;
    case 8:
        func_001AED80(1);
        break;
    case 9:
        switch (blk[4]) {
        case 0:
            func_001AEDB0(1);
            blk[4] = blk[4] + 1;
            *(float *)(rec + 0x10) = 0.0f;
            return 0;
        case 1:
            if (*(float *)(rec + 0x10) < *(float *)(rec + 0xC)) {
                *(float *)(rec + 0x10) += 1.0f;
                return 0;
            }
            break;
        }
        break;
    }
    return 1;
}
