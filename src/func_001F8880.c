// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// div-by-6 array lookup: D_00275B40 is a gp-rel (sdata) pointer to an int table;
// index by a0/6, add (a0%6)<<5. Loading the element into a temp before the add
// makes mwcc emit `addu v0,v0,v1` (array value as accumulator), matching target.
extern int *D_00275B40;

int func_001F8880(int a0) {
    int v = D_00275B40[a0/6];
    return v + ((a0 % 6) << 5);
}
