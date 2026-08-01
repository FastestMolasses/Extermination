// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Ammo/item purchase-confirm frame step. Paints the HUD bar via func_001FCBD0,
// then routes on the input-flag word D_00810E74[0]:
//   bit 0x40 -> func_0020CD40() + func_001FE8D0(), return 1
//   bit 0x20 -> func_0020CD60() + func_001FE8D0(), return 1
//   bit 0x10 -> func_0020CD60() + func_001FE8D0(), return 2
// Otherwise runs the menu-step func_001FE9A0(1, arg0+0x50, arg0+0x54); on 0 it
// returns 0, else it runs func_001FECB0 over arg0+0x48/0x4C/0x58 and maps the
// result to a status code, short-circuited to 1 whenever D_00275C58 is set.
extern void func_001FCBD0(int, int, int, int, int);
extern void func_001FE8D0(void);
extern int func_001FE9A0(int, void *, void *);
extern int func_001FECB0(int, int, void *, int, int);
extern void func_0020CD40(void);
extern void func_0020CD60(void);
extern int D_00275C58;
extern unsigned short D_00810E74[8];

int func_00226D50(char *arg0) {
    int r;

    func_001FCBD0(0x3C, 0x9B, 7, 0x28, 0x70808080);
    if (D_00810E74[0] & 0x40) {
        func_0020CD40();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x20) {
        func_0020CD60();
        func_001FE8D0();
        return 1;
    }
    if (D_00810E74[0] & 0x10) {
        func_0020CD60();
        func_001FE8D0();
        return 2;
    }
    if (func_001FE9A0(1, arg0 + 0x50, arg0 + 0x54) == 0) {
        return 0;
    }
    r = func_001FECB0(*(int *)(arg0 + 0x48), *(int *)(arg0 + 0x4C), arg0 + 0x58, 0, 0);
    if (D_00275C58 != 0) {
        return 1;
    }
    if (r == 0) {
        if (*(int *)(arg0 + 0x58) != 2) {
            return 1;
        }
        return 0;
    }
    if (r == -1) {
        return 0;
    }
    return 1;
}
