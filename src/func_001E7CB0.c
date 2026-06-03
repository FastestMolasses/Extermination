// CFLAGS: -O4,p -sdatathreshold 0
// Weapon/HUD: returns whether weapon input is currently allowed. Outside mode
// 0x13 input is always allowed (1); within it, a set of sub-states
// {4,5,7,8,9} of D_00810702 block input (0), everything else allows it.
extern unsigned char D_00810700;
extern unsigned char D_00810702;

int func_001E7CB0(void) {
    int w;

    if (D_00810700 != 0x13)
        return 1;

    w = D_00810702;
    switch (w) {
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
        return 0;
    }
    return 1;
}
