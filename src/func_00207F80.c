// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// GIF/AD packet builder: appends a GS-register packet into the per-index DMA
// cursor at D_00275670[arg0].field0x10. arg4/arg5 are the 5th/6th int args
// (incoming in $t0/$t1). 0x40/0x48 dwords pack (arg1|arg2<<16) and
// (arg3|arg4<<16) sign-extended, OR'd with a 0xFFFFFF<<32 mask.
typedef int s128 __attribute__((mode(TI)));
extern char *D_00275670;

void func_00207F80(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    char *slot;
    char *p;
    long long mask;
    long long d40, d48;

    d40 = (long long)(int)(arg1 | (arg2 << 0x10));
    d48 = (long long)(int)(arg3 | (arg4 << 0x10));
    slot = D_00275670 + arg0 * 4;
    mask = (long long)0xFFFFFF << 0x20;
    *(char *)(*(char **)(slot + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(slot + 0x10) + 4) = 0;
    *(short *)(*(char **)(slot + 0x10)) = 4;
    p = *(char **)(slot + 0x10);
    *(char **)(slot + 0x10) = p + 0x50;
    ((s128 *)p)[1] = 0;
    *(int *)(p + 0x1C) = 0x50000003;
    *(long long *)(p + 0x20) = 0x8001 | ((long long)0x44000000 << 0x20);
    *(long long *)(p + 0x28) = 0x4410;
    *(long long *)(p + 0x30) = 0x146;
    *(unsigned long long *)(p + 0x38) = (unsigned int)arg5;
    *(long long *)(p + 0x40) = d40 | mask;
    *(long long *)(p + 0x48) = d48 | mask;
}
