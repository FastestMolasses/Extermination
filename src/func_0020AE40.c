// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Emits a fixed sequence of GS/DMA packet helper calls (func_00207E40, 7-arg
// register-convention helper a0..t2 with t2=the 64-bit ld payload) bracketed by
// func_00207D00(1,0)/(1,3). Flag bits in arg2 select packet variants:
//   0x40 -> alternate first-group/last-group packets
//   0x02 -> extra packet + either func_00208AD0 (if 0x08) or func_00209280
//   ==0x20 -> 0x71D0 vs 0x7100 variant of the trailing 0x68 packet.
extern void func_00207D00(int a, int b);
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, long long t2);
extern void func_00208AD0(void *a, int b, int c);
extern void func_00209280(void *a, int b, int c, long long d, int e);

void func_0020AE40(void *a0arg, void *p, int flags)
{
    int s0_flag;
    int s4_flag;

    func_00207D00(1, 0);

    func_00207E40(1, 0x7000, 0x7B40, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x0));
    func_00207E40(1, 0x7000, 0x7F40, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x10));
    func_00207E40(1, 0x8000, 0x7B40, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x8));
    func_00207E40(1, 0x8000, 0x7F40, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x18));

    s0_flag = flags & 0x40;
    if (s0_flag) {
        func_00207E40(1, 0x7800, 0x8300, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x40));
    } else {
        func_00207E40(1, 0x7000, 0x8300, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x20));
        func_00207E40(1, 0x8000, 0x8300, 0x100, 0x80, 0x40808080, *(long long *)((char *)p + 0x28));
    }

    s4_flag = flags & 0x2;
    if (s4_flag) {
        func_00207E40(1, 0x7800, 0x7E00, 0x100, 0x80, 0x80808080, *(long long *)((char *)p + 0x78));
    }

    if (flags & 0x8) {
        func_00208AD0(a0arg, 0x1B6, 0x6E);
    } else if (s4_flag) {
        func_00209280(a0arg, 0x96, 0xB4, *(long long *)((char *)p + 0x70), 1);
    }

    func_00207D00(1, 3);

    func_00207E40(1, 0x7000, 0x8300, 0x80, 0x80, 0x80808080, *(long long *)((char *)p + 0x38));

    if (!s0_flag) {
        func_00207E40(1, 0x8780, 0x8300, 0x80, 0x80, 0x80808080, *(long long *)((char *)p + 0x30));
    }

    if (flags == 0x20) {
        func_00207E40(1, 0x71D0, 0x7900, 0x100, 0x40, 0x80808080, *(long long *)((char *)p + 0x68));
    } else {
        func_00207E40(1, 0x7100, 0x7900, 0x100, 0x40, 0x80808080, *(long long *)((char *)p + 0x68));
    }
}
