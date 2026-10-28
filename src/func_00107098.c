// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.09% NEARMISS).
// Fixes read from the original instructions:
//  - the tail was logically wrong: when D_002411D4 is set and D_002411D8 is NOT below
//    D_00241478, the original adds 0x400 to the new D_00241538 (the old C stored the
//    address of a bogus symbol D_00240400 on the opposite condition);
//  - D_0024119C is always rewritten with the larger of itself and D_00241538 (a max),
//    and D_00241538 += 0x400 (rather than a saved sum) gives the original's registers.
//
// Unless the mode word D_0024147C is 3, a change of D_00241478 since the last call
// (remembered in D_002411D8) is handled: a pending wrap flag D_002411D4 is cleared and
// bumps the base D_00241198 by 0x400; a decrease sets the wrap flag unless D_002411A0
// is set; D_002411A0 is cleared and D_002411D8 updated. Then D_00241538 = base + value
// (plus 0x400 while a wrap is pending and the value has not advanced past D_002411D8),
// and D_0024119C keeps the running maximum of D_00241538.

extern int D_0024147C;
extern int D_00241478;
extern int D_002411D8;
extern int D_002411D4;
extern int D_00241198;
extern int D_0024119C;
extern int D_002411A0;
extern int D_00241538;

void func_00107098(void)
{
    int cur, last;

    if (D_0024147C != 3) {
        cur = D_00241478;
        last = D_002411D8;
        if (cur != last) {
            if (D_002411D4 != 0) {
                D_002411D4 = 0;
                D_00241198 += 0x400;
            }
            if (cur < last) {
                if (D_002411A0 == 0)
                    D_002411D4 = 1;
            }
            D_002411A0 = 0;
            D_002411D8 = D_00241478;
        }
    }

    D_00241538 = D_00241198 + D_00241478;
    if (D_002411D4 != 0 && !(D_002411D8 < D_00241478))
        D_00241538 += 0x400;
    D_0024119C = (D_0024119C < D_00241538) ? D_00241538 : D_0024119C;
}
