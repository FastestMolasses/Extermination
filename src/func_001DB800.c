// CFLAGS: -O4,p -sdatathreshold 0
extern char D_0081C040;
extern char D_0081C041;
extern char D_0081C042;
extern char D_0081C043;

// The int parameter (decomp2 lane round 3) is never read: the only caller,
// func_001999C0, sets $a0 to a different value in each of its six arms
// (5, 4, 3, 1, 2, 0) before the call, so the original prototype had it.
void func_001DB800(int sight) {
    D_0081C043 = 0;
    D_0081C042 = 0;
    D_0081C041 = 0;
    D_0081C040 = 0;
}