// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Sets the controller button map for control type t (0, 1 or 2): eight
// scratchpad halfwords 0x70003B74..0x70003B82 receive the pad bits of the
// actions (0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1 permuted per type). Other
// types leave the map unchanged.
extern short D_70003B74;
extern short D_70003B76;
extern short D_70003B78;
extern short D_70003B7C;
extern short D_70003B7E;

void func_001AF470(unsigned char t) {
    switch (t) {
    case 0:
        D_70003B7E = 2;
        *(volatile short *)0x70003B82 = 1;
        D_70003B74 = 0x80;
        D_70003B76 = 0x40;
        D_70003B78 = 0x20;
        *(volatile short *)0x70003B7A = 0x10;
        D_70003B7C = 8;
        *(volatile short *)0x70003B80 = 4;
        break;
    case 1:
        D_70003B7E = 2;
        *(volatile short *)0x70003B82 = 1;
        D_70003B74 = 0x20;
        D_70003B76 = 0x40;
        D_70003B78 = 0x80;
        *(volatile short *)0x70003B7A = 0x10;
        D_70003B7C = 8;
        *(volatile short *)0x70003B80 = 4;
        break;
    case 2:
        D_70003B7C = 2;
        D_70003B74 = 0x80;
        D_70003B76 = 0x20;
        D_70003B78 = 0x40;
        *(volatile short *)0x70003B7A = 0x10;
        D_70003B7E = 8;
        *(volatile short *)0x70003B80 = 4;
        *(volatile short *)0x70003B82 = 1;
        break;
    }
}
