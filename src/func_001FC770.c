// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Copies the 0x18-byte template D_00264BF0 (one quadword + one doubleword) into a
// stack buffer; if the caller passed a null record pointer (a3), substitute the
// freshly-copied stack buffer, then forward to func_001FC7B0 (record passed in the
// 4th arg slot a3). The #pragma pack(8) sizes the temp at 0x18 so the struct copy
// emits lq + ld (not lq + lq); 2.3.3 schedules it byte-exact (991202 does not).
typedef unsigned __int128 u128;
#pragma pack(8)
typedef struct { u128 q0; long long d0; } Cfg;
#pragma pack()
extern void func_001FC7B0(int a0, int a1, int a2, void *a3);
extern Cfg D_00264BF0;

void func_001FC770(int a0, int a1, int a2, void *a3) {
    Cfg tmp;
    tmp = D_00264BF0;
    if (a3 == 0) {
        a3 = &tmp;
    }
    func_001FC7B0(a0, a1, a2, a3);
}
