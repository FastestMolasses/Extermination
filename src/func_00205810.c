// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00205BC0(int, int, int, int, int, int, int, int, long long, long long);
extern int func_00205C60(int, int, int);
extern int func_00205CD0(int, int, int, int);

void func_00205810(int a0, int *a1, int *a2) {
    int *s0 = a1;
    int one = 1;
    int *s1 = a2;
    int v0;
    v0 = func_00205BC0(a0, 6, 0, one, 0, 0, 0, one, 0, 0);
    v0 = func_00205C60(v0, s1[0], s1[1]);
    v0 = func_00205CD0(v0, s0[0], s0[1], 0);
    v0 = func_00205C60(v0, s1[0] + s1[2], s1[1] + s1[3]);
    func_00205CD0(v0, s0[0] + s0[2], s0[1] + s0[3], 0);
}
