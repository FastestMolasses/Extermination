// NEARMISS func_00161020  (vram 0x00161020, 0x2A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS 81.5% (mwcc233; 991202 69.0%). register-allocation-ORDER: dispatch structure, all call args, and constants are verified correct against the target, but the entry block picks a different register permutation (target keeps the state byte in $a1/$v1 and copies the entity pointer to $s0 AFTE...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern short D_0028A9A0;
extern int func_00160220(char *);
extern int func_001607D0(int);
extern void func_001749A0(char *, int, int, float);
extern void func_00174A50(char *, float);
extern int func_00174AC0(char *, int);
extern void func_001756E0(char *);
extern void func_00175900(char *, int);
extern void func_001764E0(char *);
extern void func_001796C0(char *);
extern void func_0017B5C0(char *);

void func_00161020(char *a0) {
    int st = *(unsigned char *)(a0 + 6);
    char *s0 = a0;

    switch (st) {
    case 0:
        *(unsigned char *)(s0 + 6) = (unsigned char)(st + 1);
        *(unsigned char *)(s0 + 7) = 0;
        *(int *)(s0 + 0x38) = 0;
        *(char *)(s0 + 0x25C) = 0;
        func_00174A50(s0, 12.0f);
    set_delay:
        *(short *)(s0 + 0x28) = 0x12C;
        break;
    case 1:
        if (D_0028A9A0 != 0 || func_001607D0(st) != 0 || func_00160220(s0) != 0) {
            break;
        }
        if (func_00174AC0(s0, 0) != 0) {
            *(unsigned char *)(s0 + 6) = (unsigned char)(*(unsigned char *)(s0 + 6) + 1);
            func_0017B5C0(s0);
        } else {
            int sub = *(unsigned char *)(s0 + 7);
            if (sub == 1) {
                if (*(int *)(s0 + 0x200) & 0x1000) {
                    *(unsigned char *)(s0 + 7) = 0;
                    *(short *)(s0 + 0x28) = 0x12C;
                    func_00174A50(s0, 8.0f);
                }
            } else if (sub == 0) {
                if (*(unsigned char *)(s0 + 0x236) == 0 && !(*(unsigned char *)(s0 + 0x235) & 1)) {
                    short cnt = *(short *)(s0 + 0x28);
                    *(short *)(s0 + 0x28) = (short)(cnt - 1);
                    if (cnt == 0) {
                        *(unsigned char *)(s0 + 7) = (unsigned char)(*(unsigned char *)(s0 + 7) + 1);
                        func_001749A0(s0, 0x15D, 1, 8.0f);
                    }
                }
            }
        }
        break;
    case 2:
        if (func_001607D0(st) != 0 || func_00160220(s0) != 0) {
            break;
        }
        func_00174AC0(s0, 1);
        if (*(int *)(s0 + 0x200) & 0x8000) {
            break;
        }
        if (*(float *)(s0 + 0x240) == 0.0f) {
            *(unsigned char *)(s0 + 6) = 0x63;
        } else if (*(unsigned char *)(s0 + 0x25D) == 0) {
            *(char *)(s0 + 5) = 1;
            *(unsigned char *)(s0 + 6) = 0;
            *(char *)(s0 + 0x1F0) = 1;
            *(char *)(s0 + 0x1F1) = 1;
        } else {
            *(int *)(s0 + 0x204) = 0;
        }
        break;
    case 0x63:
        *(unsigned char *)(s0 + 6) = (unsigned char)(st + 1);
        func_00174A50(s0, 8.0f);
        break;
    case 0x64:
        if (*(int *)(s0 + 0x200) & 0x8000) {
            break;
        }
        *(unsigned char *)(s0 + 6) = 1;
        *(unsigned char *)(s0 + 7) = 0;
        goto set_delay;
    default:
        break;
    }

    func_001764E0(s0);
    *(float *)(s0 + 0xB4) = *(float *)(s0 + 0xB4) + -0.2f;
    func_00175900(s0, 1);
    func_001756E0(s0);
    func_001796C0(s0);
}
