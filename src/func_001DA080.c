// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Picks two entries out of the sub-object pointer array at arg2+0x110 (count =
// byte at arg2+9, entries are 4-byte pointers, each entry's transform lives at
// (*entry)+0xC0). idx_a is the entry whose func_00102738 distance against the
// workspace transform sp90 (built from arg3 by func_00102948) is SMALLEST;
// idx_b is the entry whose distance against the global frame D_00817FC0 is
// LARGEST. Both start at entry 1 and the loop scans from entry 2 up. The two
// chosen transforms are then copied into arg0 and arg1 via func_00102948.
extern float func_00102738(void *p, int a);
extern void func_00102948(void *p, int a);
extern int D_00817FC0;

void func_001DA080(void *arg0, void *arg1, char *arg2, int arg3) {
    char sp90[16];
    float best_a;
    float best_b;
    int i;
    int idx_a;
    int idx_b;
    char *p;
    int t;

    func_00102948(sp90, arg3);
    idx_a = 1;
    best_a = func_00102738(sp90, *(int *)(arg2 + 0x114) + 0xC0);
    idx_b = 1;
    best_b = func_00102738(&D_00817FC0, *(int *)(arg2 + 0x114) + 0xC0);
    i = 2;
    p = arg2 + 8;
    for (; i < (int)*(unsigned char *)(arg2 + 9); i++) {
        float va = func_00102738(sp90, *(int *)(p + 0x110) + 0xC0);
        if (va < best_a) {
            best_a = va;
            idx_a = i;
        }
        {
            float vb = func_00102738(&D_00817FC0, *(int *)(p + 0x110) + 0xC0);
            if (!(vb <= best_b)) {
                best_b = vb;
                idx_b = i;
            }
        }
        p += 4;
    }
    t = idx_a * 4;
    t += (int)arg2;
    func_00102948(arg0, *(int *)(t + 0x110) + 0xC0);
    t = idx_b * 4;
    t += (int)arg2;
    func_00102948(arg1, *(int *)(t + 0x110) + 0xC0);
}
