// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00205BC0(int, int, int, int, int, int, int, int, long long, long long);
extern int func_00205CD0(int, int, int, int);
extern int func_00205C80(int, int, int, int, int, int);

void func_002058D0(int a0, int *a1, unsigned int a2) {
    int h;
    int six = 6;
    int one = 1;
    h = func_00205BC0(a0, six, 0, 0, 0, 0, 0, one, 0, 0);
    h = func_00205CD0(h, a1[0], a1[1], 0);
    h = func_00205C80(h, a2 & 0xFF, (a2 >> 8) & 0xFF, (a2 >> 16) & 0xFF,
                      (a2 >> 24) & 0xFF, 0);
    func_00205CD0(h, a1[0] + a1[2], a1[1] + a1[3], 0);
}
