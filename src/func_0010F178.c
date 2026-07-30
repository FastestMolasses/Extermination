// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241C88;
extern unsigned char D_00278C00[];
extern unsigned char D_00279210[];
extern unsigned char D_00279040[];
extern unsigned char D_00279110[];

extern int func_0010EFD0(void);
extern int sub__9PsIIlibkernl2000_3(void);
extern int func_0010EF38(void);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);

int func_0010F178(unsigned char *name, int flags)
{
    unsigned char *pkt = D_00278C00;
    int *fd;
    int len;
    int stat;

    if (D_00241C88 == 0)
        func_0010EFD0();

    if (sub__9PsIIlibkernl2000_3() != 0)
        return 0xFFFEFFFC;

    fd = (int *)func_0010EF38();
    if (fd == 0)
        return -19;

    for (len = 0; len < 0x400; len++) {
        if ((*(char *)(pkt + len + 4) = name[len]) == 0)
            break;
    }
    if (len == 0x400) {
        *(char *)(pkt + 0x3FF + 4) = 0;
        len = 0x3FF;
    }

    *(int *)(pkt + 0) = flags & 0x0FFFFFFF;

    if (func_0010E8A8(D_00279210, 0, 0, D_00278C00, len + 5, D_00279040, 4, 0, 0) < 0)
        return -1;

    stat = *(int *)((unsigned int)D_00279040 | 0x20000000);
    if (stat < 0) {
        fd[1] = 0;
        return stat;
    }

    fd[0] = stat;
    fd[1] = fd[1] | flags;
    return ((int)fd - (int)D_00279110) >> 4;
}
