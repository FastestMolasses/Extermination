// CFLAGS: -O4,p -sdatathreshold 0
// If func_001E2290(b) succeeds, acquire an entry via func_001EF9D0(0x80000010, 0, 1.0f);
// on success stash (byte)b at +0xD and copy a->+0x14 into entry->+0x24. Returns the entry.
extern int func_001E2290(int b);
extern char *func_001EF9D0(int handle, int slot, float weight);

char *func_001F0120(int a, int b) {
    char *e;
    if (func_001E2290(b) != 0) {
        e = func_001EF9D0(0x80000010, 0, 1.0f);
        if (e != 0) {
            *(unsigned char *)(e + 0xD) = (unsigned char)b;
            *(int *)(e + 0x24) = *(int *)(a + 0x14);
            return e;
        }
        return e;
    }
    return 0;
}
