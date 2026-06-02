// Looks up an object via func_001F6D60 and, if found, releases it.
extern char *func_001F6D60(void);
extern void func_001F66F0(char *obj);

void func_001F6E80(void) {
    char *obj = func_001F6D60();
    if (obj != 0) {
        func_001F66F0(obj);
    }
}
