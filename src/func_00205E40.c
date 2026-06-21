// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// 64-bit GS-register bit-packer (idiom: dsll32/dsrl32 zero-extend + dsll/or). Packs
// six unsigned 32-bit fields into a long long at distinct bit positions and tail-
// calls func_00205A50(a0, 8, packed). a0 is passed through unchanged; func_00205A50
// stores the low/high 32 bits of the packed value plus the int 8 into a 16-byte
// record at a0. Field bit offsets: a1<<0, a2<<2, a3<<4, a4<<14, a5<<24, a6<<34.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202 (66%). 2.3.3
// is byte-identical. Verified objdiff 100% vs build/expected/func_00205E40.o.
extern void func_00205A50(void *a0, int a1, long long a2);

void func_00205E40(void *a0, unsigned int a1, unsigned int a2, unsigned int a3,
                   unsigned int a4, unsigned int a5, unsigned int a6) {
    func_00205A50(a0, 8,
                  (long long)(unsigned int)a1
                  | ((long long)(unsigned int)a2 << 2)
                  | ((long long)(unsigned int)a3 << 4)
                  | ((long long)(unsigned int)a4 << 14)
                  | ((long long)(unsigned int)a5 << 24)
                  | ((long long)(unsigned int)a6 << 34));
}
