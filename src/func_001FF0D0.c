// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Guarded per-frame state dispatcher. When the global byte D_00282157 is 0,
// loads the current object record from *0x70003B6C and switches on its state
// byte at +8: state 0 -> func_001FF830(record->byte[0xE], record); state 1 ->
// func_001FFCD0, then if the (reloaded) state byte is 0x63, calls
// func_001FEF70 and on a non-(-1) result stores it at +0xE and clears the
// status bytes +8..+0xC; state 2 -> func_00200360; state 0x63 -> clears the
// gp-rel flag D_00275BD8 and calls func_001AB7D0.
//
// mwcc 2.3.3 matches byte-identical; the pinned 991202 build caps at 94.0%.
// Two keys: (1) D_00275BD8 must be gp-rel -> declare it as a scalar `char`
// under -sdatathreshold 8 so the `D_00275BD8 = 0` store lands in the
// func_001AB7D0 call's delay slot as `sb zero,%gp_rel(...)(gp)`; (2)
// D_00282157 is FAR from gp, so declare it as `char[]` to keep it hi/lo
// (a scalar decl would wrongly try gp-rel under threshold 8).
extern void func_001FF830(unsigned char a0, char *a1);
extern void func_001FFCD0(unsigned char a0, char *a1);
extern void func_00200360(unsigned char a0, char *a1);
extern void func_001AB7D0(unsigned char a0, char *a1);
extern int func_001FEF70(unsigned char a0);
extern char D_00282157[];
extern char D_00275BD8;

void func_001FF0D0(void) {
    char *p;
    unsigned char st;
    int r;

    if (D_00282157[0] == 0) {
        p = *(char **)0x70003B6C;
        st = *(unsigned char *)(p + 8);
        switch (st) {
        case 0:
            func_001FF830(*(unsigned char *)(p + 0xE), p);
            break;
        case 1:
            func_001FFCD0(st, p);
            if (*(unsigned char *)(*(char **)0x70003B6C + 8) == 0x63) {
                r = func_001FEF70(*(unsigned char *)(*(char **)0x70003B6C + 8));
                if (r != -1) {
                    *(char *)(*(char **)0x70003B6C + 0xE) = r;
                    *(char *)(*(char **)0x70003B6C + 8) = 0;
                    *(char *)(*(char **)0x70003B6C + 9) = 0;
                    *(char *)(*(char **)0x70003B6C + 0xA) = 0;
                    *(char *)(*(char **)0x70003B6C + 0xB) = 0;
                    *(char *)(*(char **)0x70003B6C + 0xC) = 0;
                }
            }
            break;
        case 2:
            func_00200360(st, p);
            break;
        case 0x63:
            D_00275BD8 = 0;
            func_001AB7D0(st, p);
            break;
        }
    }
}
