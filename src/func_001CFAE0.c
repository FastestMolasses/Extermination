// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001CD370(int);

typedef int Q128 __attribute__((mode(TI)));  // 128-bit
struct Body { Q128 q[4]; } __attribute__((aligned(16)));  // 0x40 bytes

struct S {
    struct Body body;   // 0x00..0x3F (qword copied)
    int f40;            // 0x40
    float f44;          // 0x44
    float f48;          // 0x48
    float f4C;          // 0x4C
    float f50;          // 0x50
    int f54;            // 0x54
};

void func_001CFAE0(struct S *dst, int a1, struct S *src, float f12, float f13, float f14, float f15) {
    dst->f44 = f12;
    dst->f4C = f13;
    dst->f48 = f14;
    dst->f50 = f15;
    dst->f54 = 0;
    dst->f40 = func_001CD370(a1);
    dst->body.q[0] = src->body.q[0];
    dst->body.q[1] = src->body.q[1];
    dst->body.q[2] = src->body.q[2];
    dst->body.q[3] = src->body.q[3];
}
