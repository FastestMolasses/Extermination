// COMPILER: eegcc
// CFLAGS: -O2

extern int DisableDmacHandler(int chan);
extern int func_0010E088(int arg);
extern int func_0010E6F8(void *buf, unsigned int magic, int arg);

extern unsigned char D_00281B00[];

int func_00119240(void)
{
    unsigned char *p = D_00281B00;
    int i;

    DisableDmacHandler(0);
    func_0010E088(0);

    do {
        if (func_0010E6F8(D_00281B00, 0x736E646E, 0) < 0) {
            return -1;
        }
        i = 0x2710;
        do {
            i--;
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
        } while (i > 0);
    } while (*(volatile int *)(p + 0x24) == 0);

    return 0;
}
