// NEARMISS func_00158810  (vram 0x00158810, 0x3B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER wall: target keeps the arg0+0x1F0 pointer (a1) live in a caller-saved register across the entire function needing only ONE saved reg (s0=arg0); mwcc233 additionally needs a second saved reg (s1) for the same value, growing the frame 0x20->0x30 and shifting every downstre...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B0FD0(char *a0, int a1, int a2);
extern void func_001C6380(char *p);
extern void func_001C5570(char *p, int *v, int a, int b);
extern int func_001576E0(char *arg0, char *arg1);
extern int func_001BA1F0(char *a);
extern void func_001B17A0(char *arg0);
extern void func_001FB9F0(int a0, int a1, int a2, int a3);
extern void func_001AFC10(char *a);
extern int D_00275470;
extern unsigned char D_00810700;
extern unsigned char D_0081076A;
extern unsigned char D_00810841;
extern unsigned char D_00810C87;
extern int D_700038A0;

void func_00158810(char *arg0) {
    unsigned char st;
    char *a1;

    st = *(unsigned char *)(arg0 + 4);
    a1 = arg0 + 0x1F0;
    switch (st) {
    case 0:
        func_001B0FD0(a1, 1, 2);
        func_001C6380(arg0);
        *(int *)(arg0 + 0x20) = 0;
        *(int *)(arg0 + 0x30) = (int)&D_00275470;
        if ((&D_00810841)[D_00810700] & (1 << *(unsigned short *)(arg0 + 0x2E))) {
            *(char *)(arg0 + 0) = 2;
            *(unsigned char *)(arg0 + 5) = 4;
            return;
        }
        if (*(unsigned char *)(arg0 + 3) == 0x2F) {
            if (D_00810C87 != 0 && D_0081076A == 0) {
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
            a2 = 0x78;
            if (*(unsigned char *)(arg0 + 3) == 0x13) {
                a2 = 0x79;
            }
            func_001C5570(arg0, &D_700038A0, a2, 1);
            *(int *)(arg0 + 0x20) = (int)&D_700038A0;
        }
        return;
    case 1: {
        unsigned char sub;
        sub = *(unsigned char *)(arg0 + 5);
        switch (sub) {
        case 4:
            break;
        case 0:
            if (*(unsigned char *)(arg0 + 3) == 0x2F && D_00810C87 != 0 && D_0081076A == 0) {
                *(unsigned char *)(arg0 + 4) = 2;
            } else {
                int r;
                r = func_001576E0(arg0, a1);
                if (r != 0) {
                    if (r == 1) {
                        *(unsigned char *)(arg0 + 5) = sub + 1;
                    } else {
                        *(unsigned char *)(arg0 + 5) = 3;
                    }
                }
            }
            break;
        case 1:
            if (func_001BA1F0(a1) != 0) {
                *(char *)(arg0 + 0xB) = 0;
                *(char *)(arg0 + 0) = 2;
                *(unsigned char *)(arg0 + 5) = sub + 1;
            }
            break;
        case 2: {
            int *q;
            q = *(int **)(arg0 + 0x20);
            if (q != 0) {
                *(char *)((char *)q + 4) = 3;
                *(int *)(arg0 + 0x20) = 0;
            }
            *(unsigned char *)(arg0 + 5) = 4;
            break;
        }
        case 3:
            if (func_001BA1F0(a1) != 0) {
                *(char *)(arg0 + 0xB) = 0;
                *(char *)(arg0 + 0) = 1;
                *(unsigned char *)(arg0 + 5) = 0;
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
    case 2: {
        unsigned char sub;
        sub = *(unsigned char *)(arg0 + 5);
        switch (sub) {
        case 4:
            if (func_001576E0(arg0, a1) != 0) {
                *(unsigned char *)(arg0 + 0) = 2;
                D_0081076A = 1;
                *(unsigned char *)(arg0 + 5) = sub + 1;
            }
            break;
        case 1:
            if (*(unsigned short *)0x70003B84 == 0) {
                *(unsigned char *)(arg0 + 5) = sub + 1;
            }
            break;
        case 2:
            if (*(unsigned short *)0x70003B84 == 0x64) {
                func_001FB9F0(0x3F3, 0x1000, 0x1000, 0x1000);
            }
            if (*(unsigned short *)0x70003B84 < 0x78) {
                if (*(unsigned char *)0x70003B91 >= 2) {
                    *(unsigned char *)(arg0 + 5) = 2;
                    *(unsigned char *)(arg0 + 4) = 1;
                    D_00810841 |= 1 << *(unsigned short *)(arg0 + 0x2E);
                }
            } else {
                *(unsigned char *)(arg0 + 5) = 2;
                *(unsigned char *)(arg0 + 4) = 1;
                D_00810841 |= 1 << *(unsigned short *)(arg0 + 0x2E);
            }
            break;
        }
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        return;
    }
    case 3:
        func_001AFC10(a1);
        return;
    }
}
