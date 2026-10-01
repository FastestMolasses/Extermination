// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code is
// linked 0x40 below where it runs), 0x6C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Role: callback (self, obj) whose runtime address 0x823580 the flame
// 0x8235F0 stores at +0x34. When bit 1 of obj[0] is clear and
// func_0021BB00(D_008102B0) returns 0, it spawns class 0x80000027 at obj
// (func_001EFE00), sets obj[0xF] = 0xC and self +0x210 = 60.
extern int func_0021BB00(void *p);
extern void func_001EFE00(int cls, unsigned char *obj);
extern char D_008102B0[];

void overlay_AREA11_func_00823540(unsigned char *self, unsigned char *obj) {
    if (!(obj[0] & 2) && func_0021BB00(D_008102B0) == 0) {
        func_001EFE00(0x80000027, obj);
        obj[0xF] = 0xC;
        *(int *)(self + 0x210) = 0x3C;
    }
}
