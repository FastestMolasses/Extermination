// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Subsystem init: registers 8 handlers via func_001D2830, then runs 5
// init passes over the D_008101D0 state block.
extern void func_001D2830(int a0, int a1);
extern void func_001C1E70(void *p);
extern void func_001C1E80(void *p);
extern void func_001C1E90(void *p);
extern void func_001C1EA0(void *p);
extern void func_001C1F50(void *p);
extern char D_008101D0[];

void func_001C1DC0(void)
{
    func_001D2830(0, 1);
    func_001D2830(1, 1);
    func_001D2830(2, 0);
    func_001D2830(0x24, 0);
    func_001D2830(0x20, 0);
    func_001D2830(0x21, 0);
    func_001D2830(0x22, 0);
    func_001D2830(0x25, 0);
    func_001C1E70(D_008101D0);
    func_001C1E80(D_008101D0);
    func_001C1E90(D_008101D0);
    func_001C1EA0(D_008101D0);
    func_001C1F50(D_008101D0);
}
