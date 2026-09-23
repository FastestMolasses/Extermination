// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m2-matching lane): the mode-0/mode-1 tail is a `switch` with
// case 1 written first. A switch `break` lowers to `b join` with its delay slot
// left as nop, exactly as the target does after the mode-1 copies; the earlier
// if/else form let mwcc retarget that `b` past the join and copy the join's
// first instruction into the slot (99.24%). Case order fixes block layout.

// Semantics: dispatch on mode arg1.
//   s3 = (arg1==2) ? 7 : 6
//   func_0018D330(arg0, &D_008102B0, arg1, s3)
//   mode 2 or 6 -> s0 = func_0018F870(...)
//   mode 5       -> func_0018D910(...); s0 = 0
//   otherwise    -> s0 = func_0018DD20(...)
//   arg0[7] = (u8)s0
//   switch: mode 0 -> func_0018C6A0(arg0+0x10, &D_008105D0, 4.0f);
//             func_0018C4B0(&D_008105D0, *(float*)(arg0+0x14), 4.0f)
//           mode 1 -> func_00102948(&D_008105E0, arg0+0x20);
//             func_00102948(&D_008105D0, arg0+0x10)
//   return s0
extern int func_0018D330(unsigned char *, void *, int, int);
extern int func_0018F870(unsigned char *, void *, int, int);
extern int func_0018D910(unsigned char *, void *, int);
extern int func_0018DD20(unsigned char *, void *, int, int);
extern int func_00102948(void *, void *);
extern int func_0018C6A0(unsigned char *, void *, float);
extern int func_0018C4B0(void *, float, float);
extern char D_008102B0[];
extern char D_008105D0[];
extern char D_008105E0[];

int func_0018D7B0(unsigned char *arg0, int arg1) {
    char *d = D_008102B0;
    int s3;
    int s0;

    if (arg1 == 2) {
        s3 = 7;
    } else {
        s3 = 6;
    }
    func_0018D330(arg0, d, arg1, s3);
    if (arg1 == 2 || arg1 == 6) {
        s0 = func_0018F870(arg0, d, arg1, s3);
    } else if (arg1 == 5) {
        func_0018D910(arg0, d, s3);
        s0 = 0;
    } else {
        s0 = func_0018DD20(arg0, d, arg1, s3);
    }
    *(unsigned char *)(arg0 + 7) = s0;
    switch (arg1) {
    case 1:
        func_00102948(D_008105E0, arg0 + 0x20);
        func_00102948(D_008105D0, arg0 + 0x10);
        break;
    case 0:
        func_0018C6A0(arg0 + 0x10, D_008105D0, 4.0f);
        func_0018C4B0(D_008105D0, *(float *)(arg0 + 0x14), 4.0f);
        break;
    }
    return s0;
}
