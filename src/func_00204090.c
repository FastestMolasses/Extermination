// Empty leaf, the sibling of func_00204080. Its caller
// sub_sceMpegGetPicture_decode_error passes one pointer (the message
// D_002734C0), so the original took one parameter it never reads: most likely
// a debug print compiled out. The old C declared it (void), which disagreed
// with its caller (decomp2 lane round 1); the bytes are the same.
void func_00204090(void *msg) {
}
