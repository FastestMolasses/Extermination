// CFLAGS: -O4,p
extern int *D_00275B40;
extern void func_001026A0(void *, int *, void *);
extern void func_001F4E20(void *, void *, float);

void func_00136C40(int a0, void *a1, void *a2, float fa0) {
    int *v0;
    float fs0;
    void *s0;
    void *s1;
    fs0 = fa0;
    s1 = a1;
    s0 = a2;
    v0 = D_00275B40 + a0;
    v0 = (int *)*v0;
    func_001026A0(s1, (int *)((char *)v0 + 0x90), s1);
    func_001F4E20(s1, s0, fs0);
}
