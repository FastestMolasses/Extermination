// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.82% NEARMISS).
// Fixes read from the original instructions:
//  - the string-copy helper func_00123418 returns its destination (strncpy shape); declaring
//    it void freed $v0 and ee-gcc coloured the request pointer there instead of $v1.
//  - the request block is reached as the copy destination minus 0x14 (the copy runs first,
//    then the three header words are stored), with the twin func_00114AC0's struct and
//    `if (r == 0)` tail.
//
// RPC wrapper: unless a request is already pending (D_00241D68) or the client is not bound
// (f24 == 0, returns -100), copies up to 0x3FF bytes of the name into the request buffer,
// fills its three header words (arg0, arg1, arg3), terminates the name and sends RPC 2;
// on success it records request 2 as pending. Returns the RPC result.

extern int D_00241D68;
struct s_b0c0 {
    char pad[0x24];
    int f24;
};
extern struct s_b0c0 D_0027B0C0;
extern int D_0027B184;
extern int D_0027C680;
extern char *func_00123418(char *dst, char *src, int n);
extern int func_0010E8A8(struct s_b0c0 *, int, int, int *, int, int *, int, int, int);

int func_00114360(int x, int a1, char *name, int a3) {
    int *p;
    int r;

    if (D_00241D68 != 0)
        return D_00241D68;
    if (D_0027B0C0.f24 == 0)
        return -0x64;

    func_00123418((char *)&D_0027B184, name, 0x3FF);
    p = &D_0027B184 - 5;
    *p = x;
    p[2] = a3;
    p[1] = a1;
    *((char *)p + 0x413) = 0;
    r = func_0010E8A8(&D_0027B0C0, 2, 1, p, 0x414, &D_0027C680, 4, 0, 0);
    if (r == 0)
        D_00241D68 = 2;
    return r;
}
