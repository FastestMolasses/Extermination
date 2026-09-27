// COMPILER: eegcc
// CFLAGS: -O2
// SDK: build {a0, a1, a2&0xFFFF} on stack, Deci2Call(-5, &local).
extern int Deci2Call(int a0, int *p);

int func_0010C8B0(int a0, int a1, int a2) {
    int local[3];
    a2 &= 0xFFFF;
    local[0] = a0;
    local[1] = a1;
    local[2] = a2;
    return Deci2Call(-5, local);
}
