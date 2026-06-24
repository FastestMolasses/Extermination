// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Init pass: stop active entities (state==4), trigger queued ones per per-slot bitmask, reset two channels, fire two more, then call subsystem init.
int func_00119810(int, int);
int func_00119890(int, int);
int func_00119978(int, int, int);
int func_0011A470(int);
int func_0011A5C8(int);
int func_001F9820(void);

struct S {
    char pad[8];
    int mask;
    char pad2[0x54];
};

extern struct S D_00281FD0[3];

void func_001F9BF0(void) {
    int i;
    int j;
    int new_var;
    int m;
    struct S *p;
    int k;

    i = 0;
    do {
        if (func_00119890(0, i) == 4) {
            func_0011A470(i);
        }
        i += 1;
    } while (i < 0x30);

    p = D_00281FD0;
    k = 0;
    do {
        m = p->mask;
        j = 0;
        do {
            new_var = m & 1;
            if (new_var) {
                func_0011A5C8(j);
            }
            j += 1;
            m = m >> 1;
        } while (j < 0x30);
        k += 1;
        p += 1;
    } while (k < 3);

    func_00119978(0, 0x3FFF, 0x3FFF);
    func_00119978(1, 0x3FFF, 0x3FFF);
    func_00119810(0, 4);
    func_00119810(1, 4);
    func_001F9820();
}
