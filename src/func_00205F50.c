// CFLAGS: -O4,p -sdatathreshold 0
void func_00205F50(unsigned long long a0) {
    *(volatile int *)0x1000A030 = (int)(a0 & 0xFFFFFFFULL);
    *(volatile int *)0x1000A020 = 0;
    *(volatile int *)0x1000A000 = 0x105;
}
