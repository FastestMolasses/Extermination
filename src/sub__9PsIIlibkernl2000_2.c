// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010CE28(void);
extern int func_0010CD58(char *buf, int len);
extern int D_00241C60;

int sub__9PsIIlibkernl2000_2(int fd, char *buf, int len)
{
    if (fd != 0)
        goto error;
    if (D_00241C60 == 0) {
        if (func_0010CE28() == 0)
            goto error;
        D_00241C60 = 1;
    }
    return func_0010CD58(buf, len);
error:
    return -1;
}
