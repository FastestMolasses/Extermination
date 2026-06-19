// COMPILER: eegcc
// CFLAGS: -O2
extern float func_001277B0(int *);

float func_00128108(int a0) {
    int s[4];
    s[0] = 3;
    s[1] = (unsigned int)a0 >> 31;
    if (a0 == 0) {
        s[0] = 2;
    } else {
        s[2] = 0x1E;
        if (s[1] != 0) {
            if (a0 == (int)0x80000000) {
                return -2147483648.0f;
            }
            s[3] = -a0;
        } else {
            s[3] = a0;
        }
        {
            unsigned int v = s[3];
            if (v <= 0x3FFFFFFFU) {
                int e = s[2];
                unsigned int t;
                do {
                    t = v << 1;
                    e--;
                    v = t;
                } while (t <= 0x3FFFFFFFU);
                s[2] = e;
                s[3] = v;
            }
        }
    }
    return func_001277B0(s);
}
