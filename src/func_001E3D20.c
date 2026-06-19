// CFLAGS: -O4,p -sdatathreshold 4
// If flag-bit 2 of the actor (a1) is clear and the global gate
// func_0021BB00(D_008102B0) returns 0, trigger event 0x80000027 on the
// actor, set its byte field +0xF to 0xC and the owner's word +0x1F0 to 0x3C.
extern int func_0021BB00(char *);
extern void func_001EFE00(int, int);
extern char D_008102B0[];

void func_001E3D20(char *a0, unsigned char *a1) {
    if ((a1[0] & 2) == 0) {
        if (func_0021BB00(D_008102B0) == 0) {
            func_001EFE00(0x80000027, (int)a1);
            a1[0xF] = 0xC;
            *(int *)(a0 + 0x1F0) = 0x3C;
        }
    }
}
