// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// func_001FAAC0(slot): release/clear a per-slot resource entry.
//   D_00282154[slot] is an "active" flag byte (one per slot).
//   D_00281FD8 is an array of 0x60-byte records; field at +0 is a 64-bit
//   handle passed to func_0011A6E8 (the release/free routine).
//   D_00281FD3 (= D_00281FD8 - 5) is a byte field inside the same record.
//   D_00282178[slot] is a parallel word table cleared alongside.
// If the slot is inactive (flag == 0) it does nothing.
//
// NOTE: only mwcc 2.3.3 (mwcps2-2.3.3-000906) reproduces the target's
// instruction scheduling here; the pinned 991202 build emits the same
// instructions in a different (86.7%) order interleaving the sq spills
// with the lui/addiu address build.

extern char D_00282154[];
extern long long D_00281FD8[];
extern char D_00281FD3[];
extern int D_00282178[];

extern void func_0011A6E8(long long);

void func_001FAAC0(int arg0) {
    char *p;

    p = &D_00282154[arg0];
    if (*p != 0) {
        func_0011A6E8(D_00281FD8[arg0 * 0xC]);
        *p = 0;
        D_00282178[arg0] = 0;
        D_00281FD3[arg0 * 0x60] = 0;
    }
}
