// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Sets a flag byte. If bit 7 of the (byte) argument is set, store the raw byte
// into a 0x100-entry table indexed by D_00810700[0]. Always record the low 7
// bits in D_00810701[0], raise the dirty flag D_00275BD8, and tail-call
// func_001FF080(0, 0x1D).
//
// 991202 leaves a delay-slot/return-arg residual; mwcc 2.3.3 byte-matches.
extern unsigned char D_00810700[8];
extern unsigned char D_00810730[0x100];
extern unsigned char D_00810701[8];
extern unsigned char D_00275BD8;
extern void func_001FF080(int, int);

void func_001FF030(unsigned char a0) {
    if (a0 & 0x80) {
        D_00810730[D_00810700[0]] = (unsigned char)a0;
    }
    D_00810701[0] = (unsigned char)(a0 & 0x7F);
    D_00275BD8 = 1;
    func_001FF080(0, 0x1D);
}
