// COMPILER: eegcc
// CFLAGS: -O2
// SDK (title path): is the handle h still valid? h[0] is the object, h[1]
// the generation it was taken at; valid while the object exists, its
// generation word +0x18 still equals h[1] and its flag bit 0 (+0x10) is set.
int func_0010EA60(int *h) {
    char *o = (char *)h[0];

    if (o == 0 || h[1] != *(int *)(o + 0x18) || !(*(int *)(o + 0x10) & 1)) {
        return 0;
    }
    return 1;
}
