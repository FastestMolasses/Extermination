// 4x4 matrix copy (0x40 bytes, dst <- src): four lq then four sq, batched,
// ascending offsets, temps in a2/a3/t0/t1.
//
// Matching notes (both required for 100%):
//  - `volatile` on src pins the load order 0..3 ahead of all stores (without
//    it mwcc's scheduler sinks the q0 load next to its store).
//  - q0..q3 are declared as extra never-passed PARAMETERS so they are
//    allocated to the next argument registers a2,a3,t0,t1 — matching CW's
//    block-copy temp choice. Locals (or an aggregate copy) get v1/a1 reuse
//    instead. Callers pass only (dst, src).
typedef unsigned __int128 uint128;

void func_00102958(uint128 *dst, volatile uint128 *src, uint128 q0, uint128 q1, uint128 q2, uint128 q3) {
    q0 = src[0];
    q1 = src[1];
    q2 = src[2];
    q3 = src[3];
    dst[0] = q0;
    dst[1] = q1;
    dst[2] = q2;
    dst[3] = q3;
}
