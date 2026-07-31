// COMPILER: eegcc
// CFLAGS: -O2

extern int func_0010CE28(void);
extern int func_0010CC08(char *buf, int len);
extern int D_00241C60;

int sub__9PsIIlibkernl2000(int fd, char *buf, int len)
{
    if (fd != 1 && fd != 2)
        goto error;
    if (D_00241C60 == 0) {
        if (func_0010CE28() == 0)
            goto error;
        D_00241C60 = 1;
    }
    return func_0010CC08(buf, len);
error:
    return -1;
}
