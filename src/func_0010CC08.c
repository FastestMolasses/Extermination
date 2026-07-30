// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010C858(void *, int);
extern void func_0010C888(void *);

extern char D_00276E10[];
extern char D_00276E40[];

int func_0010CC08(char *s, int n)
{
    unsigned int status;
    char *cd;
    char *p;
    char *pkt;
    char *d;
    int count;
    int copied;

    count = 0;
    copied = 0;
    p = s;
    {
        char *ctx = D_00276E10;
        if (*(volatile int *)(ctx + 0xC) != 0)
            return -1;
    }

    do {
        __asm__ __volatile__("di");
        __asm__ __volatile__("sync.p");
        __asm__ __volatile__("mfc0 %0, $12" : "=r"(status));
    } while ((status & 0x10000) != 0);

    pkt = (char *)((unsigned int)D_00276E40 | 0x20000000);
    {
        char *ctx = D_00276E10;
        *(char **)(ctx + 0x10) = pkt;
        *(volatile int *)(ctx + 0xC) = 1;
    }
    d = pkt + 0xC;

    while (n--) {
        if (*p == '\n') {
            *d++ = '\r';
            count++;
            if (count >= 0x100)
                break;
        }
        *d++ = *p++;
        count++;
        copied++;
        if (count >= 0x100)
            break;
    }

    cd = D_00276E10;
    *(volatile int *)(cd + 0x4) = count + 0xC;
    *(short *)(pkt + 0) = *(volatile int *)(cd + 0x4);
    if (func_0010C858(*(void * volatile *)(D_00276E10 + 0x0), *(char *)(pkt + 7)) < 0) {
        *(volatile int *)(cd + 0xC) = 0;
        __asm__ __volatile__("ei");
        return -1;
    }

    while (*(volatile int *)(cd + 0xC) != 0)
        func_0010C888(*(void * volatile *)(D_00276E10 + 0x0));

    __asm__ __volatile__("ei");
    return copied;
}
