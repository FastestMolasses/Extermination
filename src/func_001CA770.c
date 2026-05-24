// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001AF890(void *);

void func_001CA770(char *a0) {
    char *s0 = a0;
    void *ptr = *(void **)(a0 + 0x90);
    if (ptr == 0) return;
    func_001AF890(ptr);
    *(int *)(s0 + 0x90) = 0;
    *(short *)(s0 + 0x94) = -1;
}
