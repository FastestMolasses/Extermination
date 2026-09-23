// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 89.50% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. The loop is now a plain for-loop; the old WIP
// note about slti/prologue walls is superseded (both fixed by 2.3.3).
//
// Frame-task table zero: func_00121A28(slot, 0, 0x20) over the 3 slots at
// D_0028A750.

extern void func_00121A28(char *, int, int);
extern char D_0028A750;
void func_001AB650(void) {
    int i;
    char *p;

    p = &D_0028A750;
    for (i = 0; i < 3; i++) {
        func_00121A28(p, 0, 0x20);
        p += 0x20;
    }
}
