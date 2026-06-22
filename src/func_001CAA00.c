// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame update for an actor at *p: derive a speed value from the linked
// sub-object (p+0x44); if present, read its float at +0x20 and scale by 1.2x
// when below the 20.0 cap, otherwise use 20.0. Then drive func_001CA990 with
// either the local pose buffer (p+0xB0) when the state byte at +0x98 is 0xFF,
// or a gp-rel pose-table entry (D_00275B40[state]+0xC0) otherwise. Finally,
// if the +0x90 handle is set, run func_001CB3C0 on the actor.
//
// mwcc 2.3.3 matches byte-identical; the 991202 build lowers the state-byte
// guard with merged-tail branch sense (residual ~87%). D_00275B40 is a gp-rel
// (sdata) int* table, so -sdatathreshold 4 forces %gp_rel addressing.
extern void func_001CA990(int a, int b, float f);
extern void func_001CB3C0(int a);
extern int *D_00275B40;

void func_001CAA00(int p) {
    float spd;
    int q;
    unsigned char st;

    q = *(int *)(p + 0x44);
    if (q != 0) {
        spd = *(float *)(q + 0x20);
        if (spd < 20.0f) {
            spd *= 1.2f;
        }
    } else {
        spd = 20.0f;
    }
    st = *(unsigned char *)(p + 0x98);
    if (st == 0xFF) {
        func_001CA990(p, p + 0xB0, spd);
    } else {
        func_001CA990(p, D_00275B40[st & 0xFF] + 0xC0, spd);
    }
    if (*(int *)(p + 0x90) != 0) {
        func_001CB3C0(p);
    }
}
