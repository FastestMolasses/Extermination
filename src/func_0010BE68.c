// COMPILER: eegcc
// CFLAGS: -O2

extern void *RFU060(void);
extern int *func_0011FD78(void);

extern char *D_00241C64;

static __inline__ unsigned int read_cop0_status(void)
{
    unsigned int status;
    __asm__ __volatile__("mfc0 %0, $12" : "=r"(status));
    return status;
}

void *func_0010BE68(int incr)
{
    unsigned int intr_enabled;
    char *newbrk;
    char *old;

    __asm__ __volatile__("mfc0 %0, $12" : "=r"(intr_enabled));
    intr_enabled &= 0x10000;
    if (intr_enabled != 0) {
        do {
            __asm__ __volatile__("di");
            __asm__ __volatile__("sync.p");
        } while ((read_cop0_status() & 0x10000) != 0);
    }

    newbrk = D_00241C64 + incr;
    if ((unsigned int)RFU060() < (unsigned int)newbrk) {
        *func_0011FD78() = 12;
        __asm__ __volatile__("ei");
        return (void *)0xFFFFFFFF;
    }

    old = D_00241C64;
    D_00241C64 = newbrk;
    if (intr_enabled != 0)
        __asm__ __volatile__("ei");
    return old;
}
