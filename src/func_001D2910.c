// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Resource lookup by id: ids below 0x20 go to func_001D2710, ids 0x20..0x3F to
// func_001E0C60 (with the id), anything else gives 0.
extern int func_001D2710(int id);
extern int func_001E0C60(int id);

int func_001D2910(int id) {
    if (id < 0x20) {
        return func_001D2710(id);
    } else if (id < 0x40) {
        return func_001E0C60(id);
    } else {
        return 0;
    }
}
