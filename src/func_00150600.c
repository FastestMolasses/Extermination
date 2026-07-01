// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state driver dispatched on the state byte at arg0+6 (self=arg0, ent=arg1).
// State 0: bump state, set global D_0081080E = 1. State 1: once D_0081080E has
// reached 4, bump state, initialize the ent physics block (ent+0x38 = 2.0f,
// ent+0x3C = -0.0125f, ent+0x40 = -1.0f, byte ent+0x64 = 6, clears +4/+2C/+34/
// +48) and the self aim triple (self+0xB0/B4/B8 = 100/-85/-80, self+0xBC = 0),
// kicks clip 7 via anim_clip_init(self,7,0,0), marks the linked object at
// self+0x20 (+5 = 3) if present, and fires func_001FBD50(self,0x87E,0,500.0f).
// State 2 (physics running): while ent+0x38 (a velocity) is negative and the
// +2 flag is unset, once self+0xB4 <= 40 sets flag +2 and fires event 0x872;
// otherwise (>=0) while the +1 flag is unset, once self+0xB4 >= -35 sets flag +1
// and fires event 0x871. Independently, if flag 0x1000 at ent+0 is set and
// self+0xB4 <= -130, sets self+4 = 3 and propagates 3 into the linked object at
// self+0x20 (+4). Default (state > 2): no-op early return.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build reaches
// 66.1% (clean-store delay-slot wall). Key: the >2 case must be a genuine no-op
// (no state bump) so the compiler emits the shared `addiu st+1` in the beqz
// delay slot and the no-restore early-return tail the target has. objdiff 100.0.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern unsigned char D_0081080E;

void func_00150600(char *arg0, char *arg1) {
    unsigned char st;
    char *p;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        D_0081080E = 1;
        break;
    case 1:
        if (D_0081080E == 4) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 4) = 0;
            *(unsigned char *)(arg1 + 0x64) = 6;
            *(float *)(arg1 + 0x38) = 2.0f;
            *(int *)(arg1 + 0x3C) = 0xBC4CCCCD;
            *(float *)(arg1 + 0x40) = -1.0f;
            *(int *)(arg1 + 0x48) = 0;
            *(int *)(arg1 + 0x34) = 0;
            *(int *)(arg1 + 0x2C) = 0;
            *(float *)(arg0 + 0xB0) = 100.0f;
            *(float *)(arg0 + 0xB4) = -85.0f;
            *(float *)(arg0 + 0xB8) = -80.0f;
            *(int *)(arg0 + 0xBC) = 0;
            anim_clip_init((int)arg0, 7, 0.0f, 0.0f);
            p = *(char **)(arg0 + 0x20);
            if (p != 0) {
                *(char *)(p + 5) = 3;
            }
            func_001FBD50(arg0, 0x87E, 0, 500.0f);
        }
        break;
    case 2:
        if (*(float *)(arg1 + 0x38) < 0.0f) {
            int flags = *(int *)(arg1 + 4);
            if (!(flags & 2)) {
                if (*(float *)(arg0 + 0xB4) <= 40.0f) {
                    *(int *)(arg1 + 4) = flags | 2;
                    func_001FBD50(arg0, 0x872, 0, 1000.0f);
                }
            }
        } else {
            int flags = *(int *)(arg1 + 4);
            if (!(flags & 1)) {
                if (*(float *)(arg0 + 0xB4) >= -35.0f) {
                    *(int *)(arg1 + 4) = flags | 1;
                    func_001FBD50(arg0, 0x871, 0, 1000.0f);
                }
            }
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (*(float *)(arg0 + 0xB4) <= -130.0f) {
                *(char *)(arg0 + 4) = 3;
                p = *(char **)(arg0 + 0x20);
                if (p != 0) {
                    *(char *)(p + 4) = 3;
                }
            }
        }
        break;
    }
}
