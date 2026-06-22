// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Mode/state lookup: reads two global state bytes (D_00810CA4 a primary
// mode flag, D_00810CA6 a sub-state) and maps the state to a result code.
//   D_00810CA4 == 2 -> 0x35
//   D_00810CA6 == 1 -> 0x32
//   D_00810CA6 in {2,3} (i.e. (u32)(x-2) < 2) -> 0x33
//   D_00810CA6 == 4 -> 0x34
//   otherwise -> -1
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;

int func_001FEF70(void) {
    int x;

    if (D_00810CA4 == 2) {
        return 0x35;
    }
    x = D_00810CA6;
    if (x == 1) {
        return 0x32;
    }
    if ((unsigned int) (x - 2) < 2) {
        return 0x33;
    }
    if (x == 4) {
        return 0x34;
    }
    return -1;
}
