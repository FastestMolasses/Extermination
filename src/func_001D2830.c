// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Flag channel set / clear by id: ids below 0x20 go to func_001D2730, ids
// 0x20..0x3F to func_001E0C80, anything else gives 0. on (the second
// argument) passes through unchanged in a1: each callee sets the channel's
// bit when it is non-zero and clears it otherwise, and returns whether the
// bit was already set. Callers pass (3, 1) and (1, 1).
extern int func_001D2730(int id, int on);
extern int func_001E0C80(int id, int on);

int func_001D2830(int id, int on) {
    if (id < 0x20) {
        return func_001D2730(id, on);
    } else if (id < 0x40) {
        return func_001E0C80(id, on);
    } else {
        return 0;
    }
}
