// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Computes two screen coordinates from a paired short table indexed by arg0
// (stride-4 records: short at D_00265920+0, short at D_00265922+2), scales each
// by 16.0 after a bias, rounds via float_to_int, then issues a single GS/GIF
// rect setup through func_00207E40 with a packed 64-bit register value.
extern int float_to_int(float);
extern void func_00207E40(int a, int b, int c, int d, int e, int f, unsigned long long g);
extern short D_00265920[];
extern short D_00265922[];

void func_00211240(int arg0) {
    int s0;
    int a6;
    s0 = float_to_int(16.0f * (float)(D_00265920[arg0 * 2] + 0x700));
    a6 = float_to_int(16.0f * (float)((D_00265922[arg0 * 2] >> 1) + 0x790));
    func_00207E40(1, s0, a6, 0x10, 0x10, 0x80808080, 0x20042E85113221D0ULL);
}
