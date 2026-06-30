// COMPILER: eegcc
// CFLAGS: -O2
extern int func_001086F8(int s, int n);
extern void func_00108748(int s);
extern int func_00108640(int s, int n);
extern void func_00108FF8(int s, int *d);

int func_00108EA8(int s, int *out) {
    unsigned int i;
    unsigned int s0;
    int s1, s2, s4;

    i = 0;
    func_001086F8(s, 0x22);
    s0 = func_001086F8(s, 3);
    func_00108748(s);
    s4 = 0xF;
    s1 = func_001086F8(s, s4);
    func_00108748(s);
    s2 = func_001086F8(s, s4);
    func_00108748(s);
    out[0] = func_001086F8(s, 9);
    func_001086F8(s, 0x1E);
    s4 = func_001086F8(s, 3);
    out[2] = (s0 >> 2) & 1;
    out[1] = (s0 << 30) | (s1 << 15) | s2;
    if (s4) {
        do {
            func_001086F8(s, 8);
            do { i++; } while (0);
        } while (i < (unsigned int)s4);
    }
    if (func_00108640(s, 0x20) == 0x1BB) {
        out[3] = 1;
        func_00108FF8(s, out);
    } else {
        out[3] = 0;
    }
    return 1;
}
