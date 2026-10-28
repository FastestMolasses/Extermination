// NEARMISS func_001F3340  (vram 0x001F3340, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful
// (instruction sequence and .text size identical to the original). Fixes read from the original (matching wave
// 2026-10-27, was 80.89%): case 4 tested before 5/6 (source order 0,1,5,6,4); the 17/18/22 and default arms each
// store 0x78 and return; the post-loop tail is one if/else sharing the final 0x78 store; the loop is a counted
// for-loop with a break on a hit; the scratchpad vectors are relocated externs, the 0x700038B0 one volatile (the
// original reloads its middle word after storing it); local order i, step, div.
// Remaining differences: FP register colouring only in the per-step position update (the A-vector words land one
// register off); survived declaration-order, temp and operand-order changes.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *dst, void *src);
extern void func_001031E0(float *a0, float *a1);
extern int func_0019A570(float *arg0, float *arg1, int arg2, int arg3);
extern int D_700031B0[];
extern char *D_700031D0;
extern float D_700038A0[4];
extern volatile float D_700038B0[4];
extern float D_700038C0[4];

void func_001F3340(char *arg0, char *arg1, int arg2) {
    int i;
    int step;
    int div;

    switch (arg2) {
    case 0:
    case 1:
    case 5:
    case 6:
    case 4:
        step = 0xC;
        goto body;
    case 2:
    case 10:
    case 23:
        step = 6;
        goto body;
    case 3:
    case 7:
    case 8:
    case 9:
        step = 4;
        goto body;
    case 17:
    case 18:
    case 22:
        *(int *)(arg0 + 0x74) = 0xB4;
        *(int *)(arg0 + 0x78) = 1;
        return;
    default:
        *(int *)(arg0 + 0x74) = 1;
        *(int *)(arg0 + 0x78) = 1;
        return;
    }
body:
    div = 0x78 / step;
    *(int *)(arg0 + 0x74) = 0;
    func_00102948(D_700038A0, arg0);
    func_00102948(D_700038B0, arg0 + 0x10);
    for (i = 0; i < div; i++) {
        float t;
        float b4;
        b4 = D_700038B0[1];
        t = (float) step;
        D_700038B0[1] = b4 + *(float *)(arg1 + 0x48) * t;
        D_700038C0[0] = D_700038A0[0] + D_700038B0[0] * t;
        D_700038C0[1] = D_700038A0[1] + D_700038B0[1] * t;
        D_700038C0[2] = D_700038A0[2] + D_700038B0[2] * t;
        if (func_0019A570(D_700038A0, D_700038C0, 4, 0) != 0) {
            func_00102948(arg0 + 0x60, D_700031B0);
            *(float *)(arg0 + 0x50) = *(float *)(D_700031D0 + 0x24);
            *(float *)(arg0 + 0x54) = *(float *)(D_700031D0 + 0x28);
            *(float *)(arg0 + 0x58) = *(float *)(D_700031D0 + 0x2C);
            break;
        }
        func_001031E0(D_700038A0, D_700038C0);
        *(int *)(arg0 + 0x74) = *(int *)(arg0 + 0x74) + step;
    }

    if (i == div) {
        *(int *)(arg0 + 0x7C) = 1;
        *(int *)(arg0 + 0x74) = step * div;
    } else {
        *(int *)(arg0 + 0x7C) = 0;
        *(int *)(arg0 + 0x74) = *(int *)(arg0 + 0x74) + step;
        if (*(int *)(arg0 + 0x74) <= 0) {
            *(int *)(arg0 + 0x74) = 1;
        }
    }
    *(int *)(arg0 + 0x78) = 1;
}
