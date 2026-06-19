// CFLAGS: -O4,p -sdatathreshold 0
extern void RFU063(int);
extern void func_00204140(int);

int func_00204AE0(int *s) {
    func_00204140(5);
    *(volatile int *)0x1000B420 = 0;
    *(volatile int *)0x1000B410 = 0;
    *(volatile int *)0x1000B430 = 0;
    RFU063(s[0x10]);            // 0x40
    return 1;
}
