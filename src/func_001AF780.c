// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Pops a record from the bone-slot free list built by func_001AF710: while
// more than 0x1E records remain (count D_00275BCC), the count drops by one
// and the next slot pointer is taken from D_00275BD0 (which advances by one
// slot). Returns 0 when the list is down to its 0x1E reserve.
typedef unsigned __int128 s128;

extern short D_00275BCC;
extern s128 **D_00275BD0;

s128 *func_001AF780(void) {
    s128 **p;

    if (D_00275BCC > 0x1E) {
        p = D_00275BD0;
        D_00275BCC--;
        D_00275BD0 = p + 1;
        return *p;
    } else {
        return 0;
    }
}
