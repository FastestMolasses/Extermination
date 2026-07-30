// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: per-entity timer/duration selector. Dispatches on the entity kind
// byte at e+0xD through the 15-entry jr-table at 0x0026DAA0 and writes the chosen
// countdown (in frames) to out+0x36, then ORs flag 0x2000 into that same halfword.
//   kinds 0..4 -> 0x3C   kind 5 -> 0x64
//   kind 6     -> t < 10.0f ? 0x50 : 0x1E
//   kind 7     -> depends on t and on the short at e+0x94:
//                 t < 10.0f : e+0x94 == 0 -> 0x118, == 1 -> 0xB4, else 0x17C
//                 otherwise : e+0x94 == 0 -> 0x96,  == 1 -> 0x78, else 0xB4
//   kinds 8, 9, 12, 13 -> 5     kind 10 -> 0x28
//   kinds 11, 14 -> return without touching out+0x36 at all
//   any kind >= 15 -> only the 0x2000 flag is set

void func_001A91C0(unsigned char *e, unsigned char *out, float t) {
    short h;
    short v;

    switch (e[0xD]) {
    case 0:
        *(short *)(out + 0x36) = 0x3C;
        break;
    case 1:
        *(short *)(out + 0x36) = 0x3C;
        break;
    case 2:
        *(short *)(out + 0x36) = 0x3C;
        break;
    case 3:
        *(short *)(out + 0x36) = 0x3C;
        break;
    case 4:
        *(short *)(out + 0x36) = 0x3C;
        break;
    case 5:
        *(short *)(out + 0x36) = 0x64;
        break;
    case 6:
        if (t < 10.0f) {
            *(short *)(out + 0x36) = 0x50;
        } else {
            *(short *)(out + 0x36) = 0x1E;
        }
        break;
    case 7:
        if (t < 10.0f) {
            h = *(short *)(e + 0x94);
            if (h == 0) {
                v = 0x118;
            } else if (h == 1) {
                v = 0xB4;
            } else {
                v = 0x17C;
            }
            *(short *)(out + 0x36) = v;
        } else {
            h = *(short *)(e + 0x94);
            if (h == 0) {
                v = 0x96;
            } else if (h == 1) {
                v = 0x78;
            } else {
                v = 0xB4;
            }
            *(short *)(out + 0x36) = v;
        }
        break;
    case 8:
        *(short *)(out + 0x36) = 5;
        break;
    case 9:
    case 12:
    case 13:
        *(short *)(out + 0x36) = 5;
        break;
    case 10:
        *(short *)(out + 0x36) = 0x28;
        break;
    case 11:
    case 14:
        return;
    }
    *(short *)(out + 0x36) = *(short *)(out + 0x36) | 0x2000;
}
