// NEARMISS func_00171320  (vram 0x00171320, 0x344 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 83.99% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation-ORDER (entity-ptr vs picked-flag colored s1/s0 opposite of target) + minor shared-epilogue merge divergence; body/structure/control-flow fully correct.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// Entity anim-state-machine step, dispatched on the mode byte at arg0+7.
// case 0: mark clip index (arg0+0x2E)=1; if func_001607D0() is nonzero and the
// stage byte arg0+0x1F0 isn't already 0x33, call anim_matrix_dispatch and return;
// else run the matrix-pick block directly. If func_001607D0()==0, set flag 0x2F2,
// and if arg0+0x274 is set either reset the clip via
// func_001FBD50(arg0,0x17A,0,300.0f) (when no live instances remain) or advance
// mode to 10; either way fall through to the tail's matrix-pick block.
// case 10: reset flags, bump linked entity's clip index, decrement the live-
// instance counter D_00810CAA, fire func_001FBD50 for a state-transition event,
// fall into case 11.
// case 11: advance the frame counter arg0+0x276; once it reaches
// float_to_int(arg0+0x2F4) frames, reset it and (if instances remain) advance the
// stage byte and bump arg0+6; clear mode byte arg0+7. Falls to the tail.
// default (anything else): skip straight to the tail.
// tail / matrix-pick (duplicated verbatim in both the case-0 fast-path and the
// shared tail, matching the target's two separate emitted copies): unless stage
// byte arg0+0x1F0==0x33 (call anim_matrix_dispatch and return), call
// anim_matrix_dispatch then pick a matrix source: if stage byte is 0x32/0x35, or
// sub-state arg0+0x275==4, or flag arg0+0x2F2 set, copy_qw4 from
// *(D_00275B40+0x10)+0x90 into arg0+0x2A0; else copy the base translate
// (*(D_00275B40+0x10)+0xC0/C4/C8) into arg0+0x2D0/2D4/2D8.
extern void anim_matrix_dispatch(void *arg0);
extern void copy_qw4(void *dst, void *src);
extern int float_to_int(float f);
extern int func_001607D0(void);
extern void func_001FBD50(void *p, int a, int b, float f);
extern int D_00275B40;
extern short D_00810CAA[8];

void func_00171320(int arg0) {
    int done;
    unsigned char mode;
    unsigned char src;

    mode = *(unsigned char *)(arg0 + 7);
    switch (mode) {
    case 0:
        *(short *)(arg0 + 0x2E) = 1;
        if (func_001607D0() != 0) {
            if (*(unsigned char *)(arg0 + 0x1F0) == 0x33) {
                anim_matrix_dispatch((void *)arg0);
                return;
            }
            anim_matrix_dispatch((void *)arg0);
            src = *(unsigned char *)(arg0 + 0x1F0);
            done = 0;
            if (src == 0x32 || src == 0x35) {
                copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                done = 1;
            } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
                copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                done = 1;
            } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
                copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
                done = 1;
            }
            if (done == 0) {
                *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
                *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
                *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
            }
            return;
        }
        *(unsigned char *)(arg0 + 0x2F2) = 1;
        if (*(unsigned char *)(arg0 + 0x274) != 0) {
            *(short *)(arg0 + 0x2E) = 0;
            if (D_00810CAA[0] == 0) {
                *(unsigned char *)(arg0 + 0x274) = 0;
                func_001FBD50((void *)arg0, 0x17A, 0, 300.0f);
            } else {
                *(unsigned char *)(arg0 + 7) = 10;
            }
        }
        break;
    case 10:
        *(unsigned char *)(arg0 + 7) = 11;
        *(unsigned char *)(arg0 + 0x274) = 0;
        *(unsigned char *)(arg0 + 0x2F2) = 0;
        *(short *)(*(int *)(arg0 + 0x20) + 0x2E) = 1;
        D_00810CAA[0] -= 1;
        if (*(unsigned char *)(arg0 + 0x1F0) == 0x31) {
            func_001FBD50((void *)arg0, 0x5DC, 0, 300.0f);
        } else if (*(unsigned char *)(arg0 + 0x1F0) == 0x34) {
            func_001FBD50((void *)arg0, 0x5DC, 0, 300.0f);
        } else {
            func_001FBD50((void *)0x5DD, 0, 0, 300.0f);
        }
        /* fallthrough */
    case 11:
        *(short *)(arg0 + 0x276) = (short)(*(short *)(arg0 + 0x276) + 1);
        if (*(short *)(arg0 + 0x276) >= float_to_int(*(float *)(arg0 + 0x2F4))) {
            *(short *)(arg0 + 0x276) = 0;
            if (D_00810CAA[0] != 0) {
                *(unsigned char *)(arg0 + 0x1F0) = 0x33;
                *(unsigned char *)(arg0 + 6) = (unsigned char)(*(unsigned char *)(arg0 + 6) + 1);
            }
            *(unsigned char *)(arg0 + 7) = 0;
        }
        break;
    default:
        break;
    }

    if (*(unsigned char *)(arg0 + 0x1F0) == 0x33) {
        anim_matrix_dispatch((void *)arg0);
        return;
    }
    anim_matrix_dispatch((void *)arg0);
    src = *(unsigned char *)(arg0 + 0x1F0);
    done = 0;
    if (src == 0x32 || src == 0x35) {
        copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        done = 1;
    } else if (*(unsigned char *)(arg0 + 0x275) == 4) {
        copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        done = 1;
    } else if (*(unsigned char *)(arg0 + 0x2F2) != 0) {
        copy_qw4((void *)(arg0 + 0x2A0), (char *)(*(int *)(D_00275B40 + 0x10)) + 0x90);
        done = 1;
    }
    if (done == 0) {
        *(float *)(arg0 + 0x2D0) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC0);
        *(float *)(arg0 + 0x2D4) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC4);
        *(float *)(arg0 + 0x2D8) = *(float *)((char *)(*(int *)(D_00275B40 + 0x10)) + 0xC8);
    }
}
