// COMPILER: eegcc
// CFLAGS: -O2
extern void func_001029C0(void *a0);
extern void func_00102718(void *a0, void *a1, void *a2);
extern void func_00102760(void *a0, void *a1);
extern void func_00102918(void *a0, void *a1, void *a2);
extern void func_001027E0(void *a0, void *a1);

void func_00102CD0(void *a0, void *a1, void *a2, void *a3) {
    int buf[20];
    func_001029C0(&buf[0]);
    func_00102718(&buf[16], a3, a2);
    func_00102760(&buf[0], &buf[16]);
    func_00102760(&buf[8], a2);
    func_00102718(&buf[4], &buf[8], &buf[0]);
    func_00102918(&buf[0], &buf[0], a1);
    func_001027E0(a0, &buf[0]);
}
