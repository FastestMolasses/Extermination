// NEARMISS func_00158810  (vram 0x00158810, 0x3B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4).
// Improved 2026-10-27 (was 83.32%): D_00275470 is reached gp-relative
// (-sdatathreshold 4; the other globals are unknown-size arrays so they stay
// absolute); every case of the state switch ends in `break`; sub-state 4 sits
// between 2 and 3 in source order; the 0x79/0x78 argument is an if/else; the
// two identical arms of state 2 sub-state 2 are one `>= 0x78 || >= 2` guard;
// 0x70003B84/0x70003B91 are volatile relocated scratchpad externs.
// Remaining differences: register colouring only, in state 0's bit test
// (index, table base, field and the constant 1 land in a2/a1/a0/v1 in the
// original vs a0/v1/a2/a1 here); 2.3.3 permuter (2 jobs, 500 s) found nothing.
//
// CORRECTED 2026-09-27 (AREA01 wave-2 review, checked against the original
// instructions; original-instruction harness: 0 differing cases).
// State 0 calls func_001B0FD0(arg0) and func_001C6380(arg0), and stores the
// value func_001C5570(arg0, D_700038A0, 0x78/0x79, 1) returns at +0x20 (not
// the block's address). State 1 sub-states 1 and 3 and state 3 pass the record
// itself to func_001BA1F0 / func_001AFC10 (+0x1F0 is passed only to
// func_001576E0). State 2 dispatches its sub-state as 0 (func_001576E0; on a
// nonzero result +0 = 2, D_0081076A[0] = 1, +5 += 1), 1 (+5 += 1 once the halfword
// 0x70003B84 is 0) and 2 (the 0x3F3 sound at 0x64; at 0x78, or earlier once
// 0x70003B91 >= 2: +5 = 2, +4 = 1 and bit (byte +0x2E) is ORed into
// D_00810841[D_00810700[0]]). Every "+5 += 1" after a call rereads +5, as the
// original does. The case order (0, 1, 2, 3 and sub-states 0, 1, 2, 4, 3) matches the
// original layout.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003B84 0x70003B91

extern int func_001B0FD0(char *rec);
extern void func_001C6380(char *rec);
extern int func_001C5570(char *rec, float *v, int a, int b);
extern int func_001576E0(char *rec, char *work);
extern int func_001BA1F0(char *rec);
extern void func_001B17A0(char *rec);
extern void func_001FB9F0(int a0, int a1, int a2, int a3);
extern void func_001AFC10(char *rec);
extern int D_00275470;
extern unsigned char D_00810700[];
extern unsigned char D_0081076A[];
extern unsigned char D_00810841[];
extern unsigned char D_00810C87[];
extern float D_700038A0[];
extern volatile unsigned short D_70003B84[];
extern volatile unsigned char D_70003B91[];

void func_00158810(char *arg0) {
    unsigned char st;
    char *a1;

    st = *(unsigned char *)(arg0 + 4);
    a1 = arg0 + 0x1F0;
    switch (st) {
    case 0:
        func_001B0FD0(arg0);
        func_001C6380(arg0);
        *(int *)(arg0 + 0x20) = 0;
        *(int *)(arg0 + 0x30) = (int)&D_00275470;
        if ((1 << *(unsigned short *)(arg0 + 0x2E)) & D_00810841[D_00810700[0]]) {
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 5) = 4;
            break;
        }
        if (*(unsigned char *)(arg0 + 3) == 0x2F) {
            if (D_00810C87[0] != 0 && D_0081076A[0] == 0) {
                *(unsigned char *)(arg0 + 4) = 2;
            }
        }
        *(char *)(arg0 + 0) = 1;
        *(int *)0x700038A0 = 0x3F800000;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        {
            int a2;
            if (*(unsigned char *)(arg0 + 3) == 0x13) {
                a2 = 0x79;
            } else {
                a2 = 0x78;
            }
            *(int *)(arg0 + 0x20) = func_001C5570(arg0, D_700038A0, a2, 1);
        }
        break;
    case 1: {
        unsigned char sub;
        sub = *(unsigned char *)(arg0 + 5);
        switch (sub) {
        case 0:
            if (*(unsigned char *)(arg0 + 3) == 0x2F && D_00810C87[0] != 0 && D_0081076A[0] == 0) {
                *(unsigned char *)(arg0 + 4) = 2;
            } else {
                int r;
                r = func_001576E0(arg0, a1);
                if (r != 0) {
                    if (r == 1) {
                        *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
                    } else {
                        *(unsigned char *)(arg0 + 5) = 3;
                    }
                }
            }
            break;
        case 1:
            if (func_001BA1F0(arg0) != 0) {
                *(char *)(arg0 + 0xB) = 0;
                *(char *)(arg0 + 0) = 2;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 2: {
            char *q;
            q = *(char **)(arg0 + 0x20);
            if (q != 0) {
                *(char *)(q + 4) = 3;
                *(int *)(arg0 + 0x20) = 0;
            }
            *(unsigned char *)(arg0 + 5) = 4;
            break;
        }
        case 4:
            break;
        case 3:
            if (func_001BA1F0(arg0) != 0) {
                *(char *)(arg0 + 0xB) = 0;
                *(char *)(arg0 + 0) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    }
    case 2: {
        unsigned char sub;
        sub = *(unsigned char *)(arg0 + 5);
        switch (sub) {
        case 0:
            if (func_001576E0(arg0, a1) != 0) {
                *(unsigned char *)(arg0 + 0) = 2;
                D_0081076A[0] = 1;
                *(unsigned char *)(arg0 + 5) = *(unsigned char *)(arg0 + 5) + 1;
            }
            break;
        case 1:
            if (D_70003B84[0] == 0) {
                *(unsigned char *)(arg0 + 5) = sub + 1;
            }
            break;
        case 2:
            if (D_70003B84[0] == 0x64) {
                func_001FB9F0(0x3F3, 0x1000, 0x1000, 0x1000);
            }
            if (D_70003B84[0] >= 0x78 || D_70003B91[0] >= 2) {
                *(unsigned char *)(arg0 + 5) = 2;
                *(unsigned char *)(arg0 + 4) = 1;
                D_00810841[D_00810700[0]] |= 1 << *(unsigned char *)(arg0 + 0x2E);
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        break;
    }
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
