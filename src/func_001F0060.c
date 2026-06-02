extern int *func_001EF9D0(int handle, int slot, float weight);  // acquire entry

// Acquires an entry for `handle` at full weight and, if one was returned,
// records `owner` in its +0x24 field.
void func_001F0060(int handle, int owner) {
    int *entry = func_001EF9D0(handle, 0, 1.0f);
    if (entry != 0) {
        entry[9] = owner;  // +0x24
    }
}
