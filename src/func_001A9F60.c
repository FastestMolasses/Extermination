// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Iterates the object list (count D_00275B98, array D_00275B90) and for each
// entry whose flags match ((+2 & 0x1F)==2, (+0)&1 set, +3 == 0) calls
// func_001A9E00(a0). Gated off by 0x70003B8D and D_0028A9A0.
extern void func_001A9E00(int);
extern short D_0028A9A0[8];
extern short D_00275B98;
extern int D_00275B90;

void func_001A9F60(int a0)
{
    int n;
    int *p;
    int e;

    if (*(unsigned char *)0x70003B8D != 0) return;
    if (D_0028A9A0[0] != 0) return;
    p = (int *)D_00275B90;
    n = D_00275B98;
    if (n == 0) return;
    do {
        e = *p++;
        n--;
        if ((*(unsigned char *)(e + 2) & 0x1F) == 2 &&
            (*(unsigned char *)e & 1) != 0 &&
            *(unsigned char *)(e + 3) == 0) {
            func_001A9E00(a0);
        }
    } while (n != 0);
}
