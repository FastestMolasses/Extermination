// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// SEMANTICS: fetch the "aim/anchor point" for an actor into the 3-float (16-byte)
// output buffer `out`. Unless the actor's type byte p[2] masked with ~0xE0 equals 2
// the raw world position p+0xB0..0xB8 is copied straight out. For type 2 the anchor
// depends on the sub-mode byte p[3]: modes 0..11 read the C0/C4/C8 triple out of a
// sub-object pointer stored in one of the actor's slots (0x118, 0x11C, 0x120, 0x124,
// 0x130 or 0x148 - reloaded per component, matching the original), mode 16 uses the
// raw position with +6.0 added to Y, and mode 18 builds the constant quaternion
// (0, 6.0, 0, 1.0) in `out` and transforms it by the matrix at (*(p+0x110))+0x90.
// Every other mode (9, 12..15, 17 and anything >= 19) falls back to the raw position.

extern void func_001026A0(void *a, void *b, void *c);

void func_00183C40(unsigned char *p, unsigned char *out)
{
    if ((p[2] & ~0xE0) != 2) {
        *(float *)(out + 0) = *(float *)(p + 0xB0);
        *(float *)(out + 4) = *(float *)(p + 0xB4);
        *(float *)(out + 8) = *(float *)(p + 0xB8);
        return;
    }
    switch (p[3]) {
    case 0:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x118) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x118) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x118) + 0xC8);
        break;
    case 1:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x11C) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x11C) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x11C) + 0xC8);
        break;
    case 2:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x11C) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x11C) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x11C) + 0xC8);
        break;
    case 3:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x118) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x118) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x118) + 0xC8);
        break;
    case 4:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x118) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x118) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x118) + 0xC8);
        break;
    case 5:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x130) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x130) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x130) + 0xC8);
        break;
    case 6:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x124) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x124) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x124) + 0xC8);
        break;
    case 7:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x118) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x118) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x118) + 0xC8);
        break;
    case 8:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x118) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x118) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x118) + 0xC8);
        break;
    case 10:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x120) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x120) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x120) + 0xC8);
        break;
    case 11:
        *(float *)(out + 0) = *(float *)(*(int *)(p + 0x148) + 0xC0);
        *(float *)(out + 4) = *(float *)(*(int *)(p + 0x148) + 0xC4);
        *(float *)(out + 8) = *(float *)(*(int *)(p + 0x148) + 0xC8);
        break;
    case 16:
        *(float *)(out + 0) = *(float *)(p + 0xB0);
        *(float *)(out + 4) = 6.0f + *(float *)(p + 0xB4);
        *(float *)(out + 8) = *(float *)(p + 0xB8);
        break;
    case 18:
        *(int *)(out + 0) = 0;
        *(int *)(out + 4) = 0x40C00000;
        *(int *)(out + 8) = 0;
        *(int *)(out + 0xC) = 0x3F800000;
        func_001026A0(out, (unsigned char *)*(int *)(p + 0x110) + 0x90, out);
        return;
    default:
        *(float *)(out + 0) = *(float *)(p + 0xB0);
        *(float *)(out + 4) = *(float *)(p + 0xB4);
        *(float *)(out + 8) = *(float *)(p + 0xB8);
        break;
    }
}
