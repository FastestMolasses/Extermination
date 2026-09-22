// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Forwarding p to func_00180420 is required by the callee and reproduces
// the original order of the saved-parameter copies in the prologue.
//
// Logic: func_00180420((int)p) resets scratch; mirrors p+0x290/0x298 floats into
// D_700038A0. flag selects -4.5f vs +4.5f into D_700038B0 (func_001026A0
// builds it from p+0xD0). Loops i=0..1 over the 2-float table D_002754B0:
// each pass sets scratch float 0x700038A4 = (global[0x700031D0... actually
// D_00275B40->[4]+0xC4) + table[i]; func_001028B8 combines; func_00180300
// tests with p's byte[0xD]; on a zero (success) result sets bit i in mask.
// Returns 0 iff both passes succeeded (mask==3), else 1.
//
// gp-rel note: D_002754B0 must be declared with explicit size [2] and built
// with -sdatathreshold 8 so mwcc emits the gp-relative addiu the target uses.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_00180300(char *p, void *b, unsigned char n);
extern void func_00180420(int p);
extern float D_002754B0[2];
extern char *D_00275B40;
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];

int func_00180850(char *p, int flag) {
    int mask;
    int i;
    float *fp;

    mask = 0;
    func_00180420((int)p);
    *(float *)0x700038A0 = *(float *)(p + 0x290);
    *(float *)0x700038A8 = *(float *)(p + 0x298);
    if (flag == 0) {
        *(int *)0x700038B0 = 0xC0900000;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0;
    } else {
        *(int *)0x700038B0 = 0x40900000;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0;
    }
    func_001026A0(D_700038B0, p + 0xD0, D_700038B0);
    fp = D_002754B0;
    i = 0;
    do {
        *(float *)0x700038A4 = *(float *)(*(char **)(D_00275B40 + 4) + 0xC4) + *fp;
        func_001028B8(D_700038C0, D_700038B0, D_700038A0);
        if (func_00180300(p, D_700038C0, *(unsigned char *)(p + 0xD)) == 0) {
            mask |= 1 << i;
        }
        i += 1;
        fp += 1;
    } while (i < 2);
    if (mask == 3) {
        return 0;
    }
    return 1;
}
