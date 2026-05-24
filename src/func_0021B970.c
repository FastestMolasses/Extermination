extern int *D_00275670;
extern void func_0021B920(float, float);
extern void func_0021B900(void);

void func_0021B970(float f12, float f13) {
    int *v0;
    v0 = D_00275670;
    *(float *)((char *)v0 + 0xB8) = f12;
    v0 = D_00275670;
    *(float *)((char *)v0 + 0xBC) = f13;
    func_0021B920(f12, f13);
    func_0021B900();
}
