// One-shot init: run the per-frame setup, then if still in state 1 transition
// to state 4 (sub=0,flag=0), kick off an 8.0-unit blend, and snapshot the
// current clip id into the previous-clip slot.
extern void func_001662D0(unsigned char *e);
extern void func_00174A50(unsigned char *e, float blend);

void func_001838B0(unsigned char *e) {
    func_001662D0(e);
    if (e[4] == 1) {
        e[4] = 4;
        e[5] = 0;
        e[6] = 0;
        func_00174A50(e, 8.0f);
        *(short *)(e + 0x1F2) = *(short *)(e + 0x20C);
    }
}
