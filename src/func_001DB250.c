// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS:
//   Per-frame update of the parameter block at D_00818000, dispatched on the
//   progress byte at +0x4011 (aliased by the symbol D_0081C011) through
//   jtbl_0026E6D0 (7 entries).  That byte is both the switch selector and a
//   stage counter.  Stage 0 -- and any out-of-range value, which first resets
//   the byte to 0 and falls into stage 0 -- installs the initial integer set
//   0x4030/0x4034/0x4038/0x403C and the float set 0x4000/0x4004/0x4008/0x400C;
//   stage 1 installs a second float set; stage 2 resets the counter; stages 3
//   and 4 slam 0x400C to 2.0 and advance the counter; stage 5 decays
//   0x4000/0x4004/0x400C, clears 0x4008, and advances the counter once 0x400C
//   drops below 0.5; stage 6 does nothing.
//   From stage 3 onwards the four integers ease toward 0xC0 by 1/64 of the
//   remaining distance each call and the float at 0x4028 eases toward
//   452.2256 at rate 0.1.  Unless the counter has reached 6 the block is then
//   handed to func_001DB040, which reads only $a0 (its first act is to
//   clobber $a1), so the call is single-argument -- the live $a1 at the call
//   site is just the value the `!= 6` compare left behind.
//

extern char D_00818000[];
extern unsigned char D_0081C011;
extern void func_001DB040();

void func_001DB250(void)
{
    char *p;
    int st;

    p = D_00818000;
    switch (D_0081C011) {
    default:
        *(unsigned char *)(p + 0x4011) = 0;
        /* fallthrough */
    case 0:
        *(int *)(p + 0x4030) = 0x71;
        *(int *)(p + 0x4034) = 0x53;
        *(int *)(p + 0x4038) = 0x71;
        *(int *)(p + 0x403C) = 0;
        *(float *)(p + 0x4000) = 0.991f;
        *(float *)(p + 0x4004) = 0.0176f;
        *(float *)(p + 0x4008) = 0.05f;
        *(float *)(p + 0x400C) = 0.2f;
        break;
    case 1:
        *(float *)(p + 0x4000) = 0.991f;
        *(float *)(p + 0x4004) = 0.022f;
        *(float *)(p + 0x4008) = 0.05f;
        *(float *)(p + 0x400C) = 0.4f;
        break;
    case 2:
        *(unsigned char *)(p + 0x4011) = 0;
        break;
    case 3:
        *(float *)(p + 0x400C) = 2.0f;
        *(unsigned char *)(p + 0x4011) = *(unsigned char *)(p + 0x4011) + 1;
        break;
    case 4:
        *(float *)(p + 0x400C) = 2.0f;
        *(unsigned char *)(p + 0x4011) = *(unsigned char *)(p + 0x4011) + 1;
        break;
    case 5: {
        float ka = 0.9995f;
        float kb = 0.2f;
        float kc = 0.99f;
        float lim = 0.5f;

        *(float *)(p + 0x4000) *= ka;
        *(float *)(p + 0x4004) *= kb;
        *(float *)(p + 0x400C) *= kc;
        *(float *)(p + 0x4008) = 0.0f;
        if (*(float *)(p + 0x400C) < lim) {
            *(unsigned char *)(p + 0x4011) = *(unsigned char *)(p + 0x4011) + 1;
        }
        break;
    }
    case 6:
        break;
    }

    if ((int)*(unsigned char *)(p + 0x4011) >= 3) {
        *(int *)(p + 0x4030) += (0xC0 - *(int *)(p + 0x4030)) >> 6;
        *(int *)(p + 0x4034) += (0xC0 - *(int *)(p + 0x4034)) >> 6;
        *(int *)(p + 0x4038) += (0xC0 - *(int *)(p + 0x4038)) >> 6;
        *(int *)(p + 0x403C) += (0xC0 - *(int *)(p + 0x403C)) >> 6;
        *(float *)(p + 0x4028) += 0.1f * (452.2256f - *(float *)(p + 0x4028));
    }

    st = *(unsigned char *)(p + 0x4011);
    if (st != 6) {
        func_001DB040(p);
    }
}
