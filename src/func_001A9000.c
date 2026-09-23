// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B88

//
// MATCH NOTE (m2-matching lane, 94.55% -> 100.0%): the recorded
// "delay-slot-fill wall" was the literal spelling of the two scratchpad
// countdowns. As relocated volatile externs (idiom-22/32), over-declared as
// arrays so -sdatathreshold 8 keeps them absolute (idiom #20), mwcc no longer
// hoists `lui at,0x7000` into the back-edge slots. 0x70003B86 is in the global
// _SPAD_SYMS; 0x70003B88 is opted in by `// SPAD: 0x70003B88`.
//
// Two-level scan over two gp-relative object lists. Outer list: pointer array
// D_00275BA0, length D_00275BA8 (mirrored into the scratchpad counter at
// 0x70003B86 which is decremented each step). For each outer entry e whose
// header says e[3]==5, e[0xD]!=0xB and e[0]==1, scan the inner list: pointer
// array D_00275B80, length D_00275B88 (counter at 0x70003B88). For each inner
// entry o with o[0]==1, dispatch on the type byte o[3]:
//   0x1E,0x06              -> func_001A8E80(e, o)
//   0x1F,0x50,0x1C         -> func_001A8E80(e, o) unless (D_00810700==0 && D_00810702==5)
//   0x2A,0x18,0x0C,0x0A    -> func_001A8F40(e, o)
extern void func_001A8E80(int *e, unsigned char *o);
extern void func_001A8F40(int *e, unsigned char *o);
extern int **D_00275BA0;
extern short D_00275BA8;
extern int **D_00275B80;
extern short D_00275B88;
extern unsigned char D_00810700[16];
extern unsigned char D_00810702[16];
extern volatile short D_70003B86[8];
extern volatile short D_70003B88[8];

void func_001A9000(void) {
    int *e;
    int **outer;
    int **inner;
    unsigned char *o;
    unsigned char t;
    short n;

    n = D_00275BA8;
    D_70003B86[0] = n;
    if (n == 0) {
        return;
    }
    if (D_00275B88 == 0) {
        return;
    }
    outer = D_00275BA0;
    while (D_70003B86[0] != 0) {
        n = D_70003B86[0];
        e = *outer;
        D_70003B86[0] = n - 1;
        outer += 1;
        if (*(unsigned char *)((char *)e + 3) == 5 &&
            *(unsigned char *)((char *)e + 0xD) != 0xB &&
            *(unsigned char *)((char *)e + 0) == 1) {
            inner = D_00275B80;
            D_70003B88[0] = D_00275B88;
            while (D_70003B88[0] != 0) {
                n = D_70003B88[0];
                o = (unsigned char *)*inner;
                D_70003B88[0] = n - 1;
                inner += 1;
                if (o[0] == 1) {
                    t = o[3];
                    switch (t) {
                    case 0xA:
                    case 0xC:
                    case 0x18:
                    case 0x2A:
                        func_001A8F40(e, o);
                        break;
                    case 0x1C:
                    case 0x50:
                    case 0x1F:
                        if (D_00810700[0] != 0) {
                            goto call_e80;
                        }
                        if (D_00810702[0] != 5) {
                            goto call_e80;
                        }
                        break;
                    case 0x6:
                    case 0x1E:
                    call_e80:
                        func_001A8E80(e, o);
                        break;
                    }
                }
            }
        }
    }
}
