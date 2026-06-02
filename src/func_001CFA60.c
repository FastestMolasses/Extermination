// Initialises an object: seeds its transform/params block (+0x44..+0x54),
// acquires a handle, then copies a 0x40-byte source block over the object's
// head (four quadword copies).
typedef unsigned __int128 uint128;
struct Block40 { uint128 q[4]; };
extern int func_001CD370(int);

void func_001CFA60(unsigned char *obj, struct Block40 *src, float f12, float f13) {
    *(float *)(obj + 0x44) = f12;
    *(float *)(obj + 0x4C) = f13;
    *(int *)(obj + 0x48) = 0x3F800000;  /* 1.0f */
    *(int *)(obj + 0x50) = 0x358637BD;
    *(int *)(obj + 0x54) = 0;
    *(int *)(obj + 0x40) = func_001CD370(0);

    {
        struct Block40 *dst = (struct Block40 *)obj;
        dst->q[0] = src->q[0];
        dst->q[1] = src->q[1];
        dst->q[2] = src->q[2];
        dst->q[3] = src->q[3];
    }
}
