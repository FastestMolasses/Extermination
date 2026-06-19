// CFLAGS: -O4,p -sdatathreshold 0
extern int float_to_int(float);
extern int func_001C6160(void *);

int func_001C6190(void *a0) {
    int r = func_001C6160(a0);
    return r - float_to_int(*(float *)((char *)a0 + 0x3C));
}
