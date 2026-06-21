// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// If func_001B0EA0(p) is nonzero, return 1. Otherwise call
// bone_init_default_1(p), increment the byte at p+4, and return 0.
extern int func_001B0EA0(int);
extern void bone_init_default_1(int);

int func_001B0FD0(int a0)
{
    if (func_001B0EA0(a0) != 0)
        return 1;
    bone_init_default_1(a0);
    *(unsigned char *)(a0 + 4) += 1;
    return 0;
}
