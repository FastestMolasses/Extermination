// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// State-machine dispatch on the byte phase field at a[4] (a = second arg,
// $a1; first arg $a0 is unused). 5-way switch:
//   0: kick func_001FBC50 + func_001FABB0, advance phase
//   1: advance phase only if global gate D_00282157 (signed byte) is clear
//   2: publish b[0x14] into gp-rel D_00275C78, set D_00821058=1, advance phase
//   3: advance phase only if D_00821058 (unsigned byte) is clear
//   4: if b[8]==0 call func_001FAE70(1); return 1
//   else / fallthrough: return 0
//
// Matched via mwcc 2.3.3: the pinned 991202 build (90.23%) cannot merge the
// per-case increment-store into the single shared return-0 epilogue the way
// 2.3.3 does. gp-rel byte D_00275C78 declared size-1 (gp-rel at threshold 4);
// the two far globals declared as oversized arrays to force absolute (%hi/%lo)
// addressing; D_00821058 is unsigned (lbu) while D_00282157 is signed (lb).
extern void func_001FABB0(void);
extern void func_001FAE70(int);
extern void func_001FBC50(void);
extern unsigned char D_00275C78;
extern char D_00282157[8];
extern unsigned char D_00821058[8];

int func_001B7A30(int arg0, unsigned char *a, int *b)
{
    unsigned char v;

    v = a[4];
    switch (v) {
    case 0:
        func_001FBC50();
        func_001FABB0();
        a[4] = a[4] + 1;
        break;
    case 1:
        if (D_00282157[0] == 0) {
            a[4] = v + 1;
        }
        break;
    case 2:
        D_00275C78 = *(unsigned char *)((char *)b + 0x14);
        D_00821058[0] = 1;
        a[4] = a[4] + 1;
        break;
    case 3:
        if (D_00821058[0] == 0) {
            a[4] = v + 1;
        }
        break;
    case 4:
        if (b[2] == 0) {
            func_001FAE70(1);
        }
        return 1;
    }
    return 0;
}
