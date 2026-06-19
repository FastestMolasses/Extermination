// CFLAGS: -O4,p -sdatathreshold 4
extern char *D_00275670;
extern unsigned char D_00810700[8];
extern unsigned char D_00810701[8];

extern void skin_arena_init(void);
extern void func_001D9720(void);
extern void func_001DD940(void);
extern void func_001E0C30(void);
extern void func_001D9060(void);
extern void func_001D71F0(void);
extern void func_001D7BB0(void);
extern void func_001D2830(int a0, int a1);
extern void func_001D2DE0(int a0, int a1);
extern void func_001E0CC0(void);
extern void func_001E0380(void);

void func_001D19E0(void) {
    skin_arena_init();
    func_001D9720();
    func_001DD940();
    func_001E0C30();
    func_001D9060();
    func_001D71F0();
    func_001D7BB0();
    func_001D2830(2, 0);
    func_001D2830(9, 0);
    func_001D2830(0x24, 0);
    func_001D2830(5, 0);
    func_001D2DE0(1, 0x320);
    func_001D2DE0(2, 0);
    func_001D2830(7, 0);
    func_001D2830(8, 0);
    func_001E0CC0();
    func_001D2DE0(0, 0);
    *(int *)(D_00275670 + 0x1D8) = 0;
    *(int *)(D_00275670 + 0x1E8) = 0;
    if (((D_00810700[0] << 8) + D_00810701[0]) == 0x1100) {
        func_001E0380();
    }
}
