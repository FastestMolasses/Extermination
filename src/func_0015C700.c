// CFLAGS: -O4,p -sdatathreshold 0
extern float D_00810858;
extern void func_0015C7C0(void *);

void func_0015C700(void *a0) {
    float f1 = D_00810858;
    *(float *)((char *)a0 + 0x220) = f1;
    if (f1 > 35.0f) {
        *(unsigned char *)((char *)a0 + 0x235) &= 0x2;
        func_0015C7C0(a0);
    }
}
