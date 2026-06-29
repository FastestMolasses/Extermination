// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-tick handler for an entity 'a' driven by a controller 'b'. Counts down a
// timer a->f34 by an amount derived from a->f36 (low 12 bits, x5 when 0x8000 set).
// Manages cooldowns/flags on b (f38/f70/f72/f85/f89) and, when the timer expires,
// transitions the entity state (f0/f4/f5/f6) and may fire func_001EFE00 / func_001B4CF0.
// Returns 1 when an action was taken this tick, 0 when idle/blocked.
extern void func_001B4CF0(void *);
extern void func_001EFE00(int, void *);

typedef struct {
    char f0;
    char pad1[3];
    char f4;
    char f5;
    char f6;
    char pad7[0x2D];
    short f34;
    short f36;
} A;

typedef struct {
    char pad0[0x38];
    int f38;
    char pad3C[0x34];
    short f70;
    short f72;
    char pad74[0xD];
    char f81;
    char pad82[3];
    char f85;
    char f86;
    char f87;
    char pad88;
    char f89;
    char pad8A[2];
    char f8C;
} B;

int func_0014BE20(A *a, B *b)
{
    int t;
    short m;
    short delta;

    if (a->f34 == 0 || a->f36 == 0)
        return 0;

    if (b->f85 != 0 || b->f86 != 0) {
        a->f36 = 0;
        return 0;
    }

    a->f0 = 2;
    b->f38 = 0xF0;
    b->f85 = -1;

    m = a->f36;
    delta = (short)(m & 0xFFF);
    if (m & 0x8000)
        delta = (short)(delta * 5);

    if (b->f89 == 0 && (m & 0x4000)) {
        b->f89 = 0x3C;
        func_001EFE00(0x80000027, a);
    }

    if (b->f70 != 0) {
        m = a->f36 & 0xFFF;
        b->f72 = b->f72 + m;
    } else {
        b->f72 = a->f36 & 0xFFF;
    }
    b->f70 = 0x19;

    t = a->f34;
    if (t <= delta) {
    a->f34 = 0;
    a->f4 = 2;
    a->f5 = 2;
    a->f6 = 0;
    func_001B4CF0(a);
    if (b->f87 != 0)
        return 1;
    if ((b->f81 & 2) != 0) {
        if ((a->f36 & 0x2000) == 0)
            goto skip5;
    }
    a->f5 = 1;
skip5:
    return 1;
    }

    a->f34 = t - delta;
    m = a->f36;
    if (m & 0x2000) {
        a->f4 = 2;
        a->f6 = 0;
        if (b->f87 != 0)
            a->f5 = 0;
        else
            a->f5 = 1;
        return 1;
    }

    if ((unsigned char)a->f4 != 1) {
        a->f0 = 1;
        a->f36 = 0;
        return 0;
    }

    if (b->f72 >= 0x19 || (m & 0x8000)) {
        a->f4 = 2;
        a->f6 = 0;
        b->f72 = 0;
        if (b->f81 & 2)
            a->f5 = 0;
        else
            a->f5 = 1;
        return 1;
    }

    a->f0 = 1;
    b->f8C = 2;
    a->f36 = 0;
    if (a->f36 & 0x5000)
        b->f85 = 0x1E;
    else
        b->f85 = 0;
    return 0;
}
