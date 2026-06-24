// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Dispatches GS/DMA primitive uploads via func_00207E40 (7-arg call: a0..a3,
// t0, t1=0x80808080, t2=64-bit packed const hi=0x20042E85<<32 | varying lo).
// Always issues func_00207D00(1,3) first, then a pair of func_00207E40 uploads
// selected by D_00810E70 bits 0x2000/0x8000 (else default), and when arg0==1 a
// second pair selected by bits 0x1000/0x4000 (else default). The per-branch
// difference is only the low half of the packed t2 constant.
// Matched with mwcc 2.3.3: 991202 reaches 82.7% (it reorders the constant-load
// scheduling around each packed call); 2.3.3 matches the target ordering 100.0.
extern void func_00207D00(int, int);
extern void func_00207E40(int, int, int, int, int, int, long long);
extern unsigned short D_00810E70;

void func_00210C00(int arg0) {
    func_00207D00(1, 3);
    if (D_00810E70 & 0x2000) {
        func_00207E40(1, 0x8D50, 0x7D80, 0x20, 0x20, 0x80808080, 0x55322178LL | (0x20042E85LL << 32));
        func_00207E40(1, 0x70B0, 0x7D80, 0x20, 0x20, 0x80808080, 0x5532215CLL | (0x20042E85LL << 32));
    } else if (D_00810E70 & 0x8000) {
        func_00207E40(1, 0x8D50, 0x7D80, 0x20, 0x20, 0x80808080, 0x55322158LL | (0x20042E85LL << 32));
        func_00207E40(1, 0x70B0, 0x7D80, 0x20, 0x20, 0x80808080, 0x5532217CLL | (0x20042E85LL << 32));
    } else {
        func_00207E40(1, 0x8D50, 0x7D80, 0x20, 0x20, 0x80808080, 0x55322158LL | (0x20042E85LL << 32));
        func_00207E40(1, 0x70B0, 0x7D80, 0x20, 0x20, 0x80808080, 0x5532215CLL | (0x20042E85LL << 32));
    }
    if (arg0 == 1) {
        if (D_00810E70 & 0x1000) {
            func_00207E40(1, 0x7F00, 0x7960, 0x20, 0x20, 0x80808080, 0x55322180LL | (0x20042E85LL << 32));
            func_00207E40(1, 0x7F00, 0x81A0, 0x20, 0x20, 0x80808080, 0x5532216CLL | (0x20042E85LL << 32));
        } else if (D_00810E70 & 0x4000) {
            func_00207E40(1, 0x7F00, 0x7960, 0x20, 0x20, 0x80808080, 0x55322168LL | (0x20042E85LL << 32));
            func_00207E40(1, 0x7F00, 0x81A0, 0x20, 0x20, 0x80808080, 0x55322184LL | (0x20042E85LL << 32));
        } else {
            func_00207E40(1, 0x7F00, 0x7960, 0x20, 0x20, 0x80808080, 0x55322168LL | (0x20042E85LL << 32));
            func_00207E40(1, 0x7F00, 0x81A0, 0x20, 0x20, 0x80808080, 0x5532216CLL | (0x20042E85LL << 32));
        }
    }
}
