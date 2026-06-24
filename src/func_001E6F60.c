// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// GIFtag/DMA packet builder. Indexes the gp-rel pool-base table D_00275670
// (array of per-index struct pointers) by arg0; the entry's +0x10 field is a
// bump-allocator cursor. Stamps the new record header (byte +3 = 0x10, word
// +4 = 0, half +0 = 6), advances the cursor by 0x70, then fills a 0x70-byte
// GIFtag block: zero qword at +0x10, GIFtag-word 0x50000005 at +0x1C, and a
// run of doublewords (the GS register-descriptor stream) at +0x20..+0x68.
// The last three doublewords pack caller-supplied fields: +0x58 = arg6
// (zero-extended), +0x60 = (arg5<<32) | (int)(arg1 | arg2<<16), and
// +0x68 = (arg5<<32) | (int)(arg3 | arg4<<16). Returns void.
//
// Built with mwcc 2.3.3: 991202 reschedules the long 64-bit OR / store chain
// and caps at 71.7%. The function genuinely has NO return value (the target
// ends with sd a0,0x68(t8) in the jr delay slot and no addiu v0) - declaring
// it void was the byte-exact key. &D_00275670[arg0] (array-index form) makes
// mwcc load the gp-rel base before the index multiply (CW operand order);
// the merged words use (long long)(int) casts to emit the dsll32/dsra32
// sign-extend idiom. Verified objdiff 100.0% vs build/expected.
typedef unsigned __int128 u128;
extern char **D_00275670;

void func_001E6F60(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6) {
    char **t3;
    char *t8;
    long long w60, w68;

    w60 = ((long long)arg5 << 32) | (long long)(int)(arg1 | (arg2 << 16));
    w68 = ((long long)arg5 << 32) | (long long)(int)(arg3 | (arg4 << 16));

    t3 = (char **)&D_00275670[arg0];

    *(char *)(t3[4] + 3) = 0x10;
    *(int *)(t3[4] + 4) = 0;
    *(short *)(t3[4] + 0) = 6;

    t8 = t3[4];
    t3[4] = t8 + 0x70;

    *(u128 *)(t8 + 0x10) = 0;
    *(int *)(t8 + 0x1C) = 0x50000005;
    *(long long *)(t8 + 0x20) = (long long)0x8001 | ((long long)0x10000000 << 32);
    *(long long *)(t8 + 0x28) = 0xE;
    *(long long *)(t8 + 0x30) = (long long)0x68 | ((long long)0x80 << 32);
    *(long long *)(t8 + 0x38) = 0x42;
    *(long long *)(t8 + 0x40) = (long long)0x8001 | ((long long)0x44000000 << 32);
    *(long long *)(t8 + 0x48) = 0x4410;
    *(long long *)(t8 + 0x50) = 0x46;
    *(long long *)(t8 + 0x58) = (long long)(unsigned int)arg6;
    *(long long *)(t8 + 0x60) = w60;
    *(long long *)(t8 + 0x68) = w68;
}
