// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame player FOOTSTEP slice (FINDINGS "FOOTSTEP SURFACE TABLE").
// Dispatches on the action byte +0x1F0:
//   - 1/2/0x2F/0x41 (locomotion): reads the per-anim property row
//     D_00248C90[idx*12] frameA (+2) / frameB (+4) (both must be nonzero); a
//     3-state step phase +0x25E (0 -> fire at clip time +0x3C <= frameA, left
//     foot; 1 -> fire at <= frameB, right foot; else re-arm when gait +0x25C
//     is 0 or +0x200 & 0xB000). Each fire = func_00182430(self, gait +0x25C)
//     surface+gear sounds, then func_00187EE0(self, footL, footR) decal/FX
//     with the skeleton foot nodes (*D_00275B40)+0x44 / +0x48.
//   - 0x36/0x37 (melee): consume the +0x25E mailbox 0x80|n -> func_00182430.
//   - default: consume the mailbox and also fire the decal with actor
//     +0xB0/+0xD0.
//   Tail (every frame): wet-feet timer +0x212 = 120 while floor attr +0x23A
//   is 6 or 0x5B, else decrement; wade layer while +0x23C != 0 and moving:
//   ripple effect 0x8000001D + level func_001E8B90(pos, 0.3*speed), gated off
//   when D_00810700 == 0x15.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: the
// +0x1F0 dispatch is a switch whose case-tests are emitted in reverse source
// order, reproducing the target's sequential `beq` chain (mwcc would coalesce
// an if/|| chain into a range check); the wall-#13 delay-slot residual under
// 991202 (the beq;nop slots in that chain plus the moving-branch lowering) is
// fixed by 2.3.3. -sdatathreshold 4 puts the 4-byte int* D_00275B40 in
// gp-relative sdata while the [8] arrays stay absolute, matching the original
// addressing split. Verified objdiff 100% vs build/expected/func_00187350.o
// (991202 = 93.0%).
extern void func_00182430(char *self, int gait);
extern void func_00187EE0(char *self, char *footA, char *footB);
extern void func_001031E0(void *dst, void *src);
extern void func_001E8B90(void *pos, float f);
extern void func_001EFD90(int id, void *pos, void *normal);
extern int func_00122BB8(void);
extern int *D_00275B40;
extern short D_00248C92[8];
extern short D_00248C94[8];
extern unsigned char D_00810700[8];
extern float D_700038A0[];

void func_00187350(char *arg0) {
    int action;
    int idx;
    int frameA;
    int frameB;
    int phase;
    char *node;
    float spd;

    action = *(unsigned char *)(arg0 + 0x1F0);
    switch (action) {
    case 0x1:
    case 0x2:
    case 0x2F:
    case 0x41:
        idx = *(short *)(arg0 + 0x20C);
        frameA = *(short *)((char *)D_00248C92 + idx * 12);
        if (frameA != 0) {
            frameB = *(short *)((char *)D_00248C94 + idx * 12);
            if (frameB != 0) {
                phase = *(unsigned char *)(arg0 + 0x25E);
                if (phase == 0) {
                    if (*(float *)(arg0 + 0x3C) <= (float)frameA) {
                        func_00182430(arg0, *(unsigned char *)(arg0 + 0x25C));
                        node = *(char **)((char *)D_00275B40 + 0x44);
                        func_00187EE0(arg0, node + 0xC0, node + 0x90);
                        *(char *)(arg0 + 0x25E) = 1;
                    }
                } else if (phase == 1) {
                    if (*(float *)(arg0 + 0x3C) <= (float)frameB) {
                        func_00182430(arg0, *(unsigned char *)(arg0 + 0x25C));
                        node = *(char **)((char *)D_00275B40 + 0x48);
                        func_00187EE0(arg0, node + 0xC0, node + 0x90);
                        *(char *)(arg0 + 0x25E) = 2;
                    }
                } else {
                    if (*(unsigned char *)(arg0 + 0x25C) == 0) {
                        *(char *)(arg0 + 0x25E) = 0;
                    }
                    if (*(int *)(arg0 + 0x200) & 0xB000) {
                        *(char *)(arg0 + 0x25E) = 0;
                    }
                }
            }
        }
        break;
    case 0x36:
    case 0x37:
        phase = *(unsigned char *)(arg0 + 0x25E);
        if (phase & 0x80) {
            func_00182430(arg0, phase & 0xF);
        }
        *(char *)(arg0 + 0x25E) = 0;
        break;
    default:
        phase = *(unsigned char *)(arg0 + 0x25E);
        if (phase & 0x80) {
            func_00182430(arg0, phase & 0xF);
            func_00187EE0(arg0, arg0 + 0xB0, arg0 + 0xD0);
        }
        *(char *)(arg0 + 0x25E) = 0;
        break;
    }

    /* wet-feet timer tail */
    {
        int attr = *(unsigned char *)(arg0 + 0x23A);
        if (attr == 6 || attr == 0x5B) {
            *(short *)(arg0 + 0x212) = 0x78;
        } else if (*(short *)(arg0 + 0x212) != 0) {
            *(short *)(arg0 + 0x212) = *(short *)(arg0 + 0x212) - 1;
        }
    }

    /* wade ripple */
    if (D_00810700[0] != 0x15 && *(unsigned char *)(arg0 + 0x23C) != 0) {
        int moving = *(unsigned char *)(arg0 + 0xA);
        if (moving != 0) {
            if (*(unsigned char *)(arg0 + 0x314) & 1) {
                if ((*(int *)0x70003B68 & 3) == 0) {
                    spd = *(float *)(arg0 + 0x38);
                } else {
                    spd = 0.0f;
                }
            } else {
                spd = *(float *)(arg0 + 0x38);
            }
        } else {
            spd = 0.3f * *(float *)(arg0 + 0x38);
        }
        if (spd != 0.0f) {
            if (moving != 0) {
                if ((*(int *)0x70003B68 & 3) == 0) {
                    if (func_00122BB8() < 0x1FFFFFFF) {
                        func_001031E0(D_700038A0, arg0 + 0xB0);
                        *(float *)0x700038A4 = *(float *)(arg0 + 0x250);
                        func_001EFD90(0x8000001D, D_700038A0, arg0 + 0xC0);
                    }
                }
            }
            func_001E8B90(arg0 + 0xB0, 0.3f * spd);
        }
    }
}
