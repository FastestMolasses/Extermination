// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Saves a0, calls func_00102948(a0+0x20, a1+0xA0), then
// *(float*)(a0+0x24) += 17.0f, then func_00102948(&D_008105E0, a0+0x20).
// The 17.0f constant is materialized via lui 0x4188 / mtc1; the second
// arg pointer (&D_008105E0) uses a %hi/%lo reloc pair (-sdatathreshold 0).
extern void func_00102948(int, int);
extern float D_008105E0;

void func_00191530(int a0, int a1)
{
    func_00102948(a0 + 0x20, a1 + 0xA0);
    *(float *)(a0 + 0x24) += 17.0f;
    func_00102948((int)&D_008105E0, a0 + 0x20);
}
