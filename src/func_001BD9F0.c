// NEARMISS func_001BD9F0  (vram 0x001BD9F0, 0x2AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.64% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NEARMISS: 6-way state-machine switch (arg0+4) with nested 5-way switch on arg0+5, fully recovered logic (verified against .s: state 3/2 early-return via func_001AFC10, state 0 init via func_001B0FD0/func_001C6380, state 1 sub-state machine driving func_00158590/func_001BA1F0/func_001BC150/func_00...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// Corrected 2026-10-01 against the original instructions (docs/FINDINGS.md "NEARMISS body
// corrections from the level side-track lanes"): func_00158590 gets (self, 1, 1), or
// (self, 0, 1) when +0x2E is 1 and D_00810774 is 1; func_001AFC10 and func_001B0FD0 get self
// (a0 is never rewritten before those calls). The earlier text passed constants.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00158590(char *self, int a1, int mode);
extern void func_001AEBA0(int);
extern void func_001AFC10(char *self);
extern int func_001B0FD0(char *self);
extern void func_001B17A0(char *);
extern int func_001BA1F0(char *);
extern void func_001BC150(char *);
extern int func_001BC860(char *, char *);
extern void func_001C6380(char *);
extern char D_00275610[];
extern float D_008105E0;
extern float D_008105E4;
extern float D_008105E8;
extern unsigned char D_008106B8;
extern unsigned char D_00810774;

void func_001BD9F0(char *arg0) {
    char *s0;
    char *s1;
    unsigned char st;
    unsigned char st2;
    short t;

    st = *(unsigned char *)(arg0 + 4);
    s0 = *(char **)(arg0 + 0x18);
    s1 = arg0 + 0x1F0;
    if (st == 3) {
        func_001AFC10(arg0);
        return;
    }
    if (st == 2) {
        func_001AFC10(arg0);
        return;
    }
    switch (st) {
    case 0:
        func_001B0FD0(arg0);
        *(short *)(arg0 + 0x34) = *(unsigned char *)(arg0 + 0x2E);
        if (*(unsigned char *)(arg0 + 3) == 3) {
            *(short *)(arg0 + 0x2E) = 1;
        } else {
            *(short *)(arg0 + 0x2E) = 0;
        }
        *(char **)(arg0 + 0x30) = D_00275610;
        func_001C6380(arg0);
        *arg0 = 1;
        return;
    case 1:
        st2 = *(unsigned char *)(arg0 + 5);
        switch (st2) {
        case 0:
            if (*(unsigned short *)(arg0 + 0x2E) == 1) {
                if (D_00810774 == 1) {
                    *arg0 = 2;
                    func_00158590(arg0, 0, 1);
                } else {
                    *arg0 = 1;
                    func_00158590(arg0, 1, 1);
                }
            } else {
                func_00158590(arg0, 1, 1);
            }
            if (func_001BC860(arg0, s1) != 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 1:
            func_00158590(arg0, 1, 1);
            if (func_001BA1F0(arg0) != 0) {
                if (*(unsigned char *)(arg0 + 3) == 4) {
                    s0 = *(char **)(s0 + 0x18);
                }
                D_008105E0 = *(float *)(s0 + 0xB0);
                D_008105E4 = 16.0f + *(float *)(s0 + 0xB4);
                D_008105E8 = *(float *)(s0 + 0xB8);
                *(unsigned char *)(s0 + 0xB) = 1;
                *(short *)(arg0 + 0x28) = 0x3C;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 2:
            func_00158590(arg0, 1, 1);
            t = *(short *)(arg0 + 0x28) - 1;
            *(short *)(arg0 + 0x28) = t;
            if (t == 0) {
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                func_001BC150(arg0);
            }
            break;
        case 3:
            func_00158590(arg0, 1, 1);
            *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            func_001AEBA0(4);
            break;
        case 4:
            func_00158590(arg0, 1, 1);
            if (D_008106B8 == 0) {
                *(unsigned char *)(arg0 + 5) = 0;
                *arg0 = 1;
                *(unsigned char *)(arg0 + 0xB) = 0;
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
}
