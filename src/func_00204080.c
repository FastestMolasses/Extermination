// Empty leaf. Every caller passes one pointer (a message such as D_00273490,
// D_002734A0 or D_002734E0 from the movie-stream code), so the original took
// one parameter it never reads: most likely a debug print compiled out. The
// old C declared it (void), which disagreed with its callers (decomp2 lane
// round 1); the bytes are the same.
void func_00204080(void *msg) {
}
