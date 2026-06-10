// Placed-prop world-matrix build (state-0 init tail of the generic prop/
// pickup behavior func_001C4820, also called by the door-assembly init
// func_001BD9F0). Composes the actor's TRS world matrix at +0xD0 from
// pos(+0xB0)/rot(+0xC0)/scale(+0x60), then copies it into all of the
// actor's per-bone matrix slots (pointer array at +0x110, one per bone,
// bone count byte at +0xC) so a rigid prop's whole skeleton carries the
// placement transform.
extern void build_trs_matrix(void *mtx, void *pos, void *rot, void *scale);
extern void func_001C9610(void *slots, int nbones, void *mtx);

void func_001C6380(unsigned char *self) {
    build_trs_matrix(self + 0xD0, self + 0xB0, self + 0xC0, self + 0x60);
    func_001C9610(self + 0x110, self[0xC], self + 0xD0);
}
