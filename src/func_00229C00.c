// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Per-tick dispatcher: increments the counter self+0x10, then on the mode
// byte self+3: 0 -> func_00229A70(self); 1 -> self[0] = 3; 2 -> self[0x16] =
// func_00229A00(self). Always returns 0 (its caller func_0022A460 stores that
// into o[0x16]). Case 2 has its own `return 0`: the original gives that arm a
// separate exit instead of sharing the switch's tail.
extern int func_00229A00(char *arg0);
extern int func_00229A70(char *arg0);

int func_00229C00(char *self) {
    *(int *)(self + 0x10) += 1;
    switch (*(unsigned char *)(self + 3)) {
    case 0:
        func_00229A70(self);
        break;
    case 1:
        *(char *)(self + 0) = 3;
        break;
    case 2:
        *(char *)(self + 0x16) = func_00229A00(self);
        return 0;
    }
    return 0;
}
