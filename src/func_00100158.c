// COMPILER: eegcc
// CFLAGS: -O2
extern void *func_00100268(void);
extern unsigned int SetCPUTimer(int a);
extern void func_0010C290(int a);
extern void AddIntcHandler2(int a, int b);
extern void SetGsCrt(int a, int b, int c);

void func_00100158(short mode, short a1, short a2, short a3) {
    short s1 = a1;
    short s2 = a2;
    short s3 = a3;
    unsigned char *s0;

    switch (mode) {
    case 0:
        s0 = (unsigned char *)func_00100268();
        *(volatile unsigned long *)0x12001000 = 0x200;
        *(unsigned short *)(s0 + 0) = s1;
        *(unsigned short *)(s0 + 2) = s2;
        *(unsigned short *)(s0 + 6) =
            (unsigned short)((((unsigned long)*(volatile unsigned long *)0x12001000 >> 16) & 0xFF) & 0xFFFF);
        SetCPUTimer(0xFF00);
        *(unsigned short *)(s0 + 4) = (s3 != 0);
        if (*(int *)(s0 + 8) != 0) {
            func_0010C290(2);
            AddIntcHandler2(2, *(int *)(s0 + 0xC));
            *(int *)(s0 + 8) = 0;
            *(int *)(s0 + 0xC) = 0;
        }
        SetGsCrt(s1 & 1, s2 & 0xFF, s3 & 1);
        return;
    case 1:
        *(volatile unsigned long *)0x12001000 = 0x100;
        return;
    default:
        return;
    }
}
