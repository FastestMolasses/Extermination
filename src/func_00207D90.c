// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// GIFtag/DMA packet builder. Picks the slot's write pointer from the table
// D_00275670[slot] (gp-relative, 4-byte ptr), writes a 0x40-byte descriptor
// at *(slot+0x10) and bumps the tail by 0x40:
//   +0x3 (byte) = 0x10   header type
//   +0x4 (word) = 0
//   +0x0 (half) = 3
//   +0x10 (qword) = 0    (sq zero)
//   +0x1C (word) = 0x50000002
//   +0x20 (dword) = 0x8001 | (0x10000000 << 32)
//   +0x28 (dword) = 0xE
//   +0x30 (dword) = packed register field
//   +0x38 (dword) = 0x40
// packed = (s32)b0 | ((s32)b1 << 16) | ((s64)(s32)b2 << 32) | ((s64)(s32)b3 << 48);
// each lane is sign-extended (dsll32/dsra32) before being shifted into place,
// and the OR tree is grouped ((b2<<32)|(b0|(b1<<16)))|(b3<<48) to match the
// target's eval order. The qword zero is one sq via unsigned __int128.
// -sdatathreshold 4 puts the 4-byte D_00275670 in gp-relative sdata.
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the 991202 build schedules the
// header stores / zero-extensions differently (83.09%); 2.3.3 reproduces the
// exact list-schedule. objdiff 100.0% byte-identical.
extern char *D_00275670;

void func_00207D90(int slot, int b0, int b2, int b1, int b3) {
    char *entry;
    char *p;
    long long packed;

    entry = D_00275670 + (slot << 2);
    *(char *)(*(char **)(entry + 0x10) + 0x3) = 0x10;
    *(int *)(*(char **)(entry + 0x10) + 0x4) = 0;
    *(short *)(*(char **)(entry + 0x10) + 0x0) = 3;
    p = *(char **)(entry + 0x10);
    *(char **)(entry + 0x10) = p + 0x40;

    packed = (((long long)(int)b2 << 32)
              | ((long long)(int)b0
                 | ((long long)(int)b1 << 16)))
             | ((long long)(int)b3 << 48);

    *(unsigned __int128 *)(p + 0x10) = 0;
    *(int *)(p + 0x1C) = 0x50000002;
    *(long long *)(p + 0x20) = 0x8001LL | ((long long)0x10000000 << 32);
    *(long long *)(p + 0x28) = 0xE;
    *(long long *)(p + 0x30) = packed;
    *(long long *)(p + 0x38) = 0x40;
}
