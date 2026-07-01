// NEARMISS func_001BB860  (vram 0x001BB860, 0x274 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Deep register-allocation + branch-shape permutation across the nested state-dispatch (target uses forward-beq out-of-line case bodies; mwcc233 emits bne/fallthrough inlined bodies) plus FP/GP register coloring in the bit-test and distance computation. Not the clean-store nop; multiple structural ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011E748(float d);
extern void func_001AFC10(char *arg0, char *a1, int a2);
extern void func_001B1DE0(char *self);
extern void func_001BB520(char *self);
extern int func_001BB560(char *arg0, char *arg1, int arg2);
extern int func_001BB7C0(void);
extern int func_001BB7F0(char *arg0);
extern void func_001BC150(char *self);
extern void func_001C6380(char *self);

extern float D_00810350;
extern float D_00810354;
extern float D_00810358;
extern unsigned char D_00810700;
extern unsigned char D_00810841;

void func_001BB860(char *arg0, int arg3) {
    char *self = arg0;
    char *ex = self + 0x1F0;
    unsigned char st = *(unsigned char *)(self + 4);
    unsigned char st2;
    unsigned char t;

    if (st == 3) goto afc;
    if (st == 2) goto afc;
    if (st == 1) goto s1;
    if (st == 0) goto s0;
    return;

s0:
    // enter: kick off the encounter and advance to state 1
    func_001BB520(self);
    *(unsigned char *)self = 1;
    return;

s1:
    st2 = *(unsigned char *)(self + 5);
    if (st2 == 4) {
        if (func_001BB7F0(self) != 0) {
            *(unsigned char *)(self + 5) = 0;
        }
        goto tail;
    }
    if (st2 == 3) {
        func_001BC150(self);
        *(unsigned char *)(self + 5) = (unsigned char)(*(unsigned char *)(self + 5) + 1);
        goto tail;
    }
    if (st2 == 2) {
        if (func_001BB7C0() != 0) {
            *(unsigned char *)(self + 5) = (unsigned char)(*(unsigned char *)(self + 5) + 1);
        }
        goto tail;
    }
    if (st2 == 1) {
        if (func_001BB7C0() != 0) {
            *(unsigned char *)(self + 0xB) = 0;
            *(unsigned char *)(self + 5) = 0;
        }
        goto tail;
    }
    if (st2 != 0) goto tail;

    // sub-state 0: pick a target line-of-sight check based on the actor kind
    // at self+3, then advance the sub-state on success
    t = *(unsigned char *)(self + 3);
    if (t == 0x16 || t == 0x17 || t == 0x3E) {
        if (*(&D_00810841 + D_00810700) & (1 << *(short *)(self + 0x34))) {
            if (func_001BB560(self, ex, 0) != 0) {
                *(unsigned char *)(self + 5) = 2;
            }
        } else if (func_001BB560(self, ex, 1) != 0) {
            *(unsigned char *)(self + 5) = (unsigned char)(*(unsigned char *)(self + 5) + 1);
        }
    } else if (func_001BB560(self, ex, 0) != 0) {
        *(unsigned char *)(self + 5) = 2;
    }

tail:
    func_001C6380(self);
    (*(void (**)(char *))(self + 0x4C))(self);

    {
        // squared distance from the player position (D_00810350/54/58) to self
        float dx = D_00810350 - *(float *)(self + 0xB0);
        float dy = D_00810354 - *(float *)(self + 0xB4);
        float dz = D_00810358 - *(float *)(self + 0xB8);
        float d = func_0011E748(dy * dy + dx * dx + dz * dz);
        if (d <= 20.0f) {
            *(unsigned char *)(self + 1) = 1;
            if (*(unsigned char *)(self + 2) & 0x80) {
                func_001B1DE0(self);
                return;
            }
        }
    }
    return;

afc:
    // transition-in-progress: hand off to the shared teardown/setup dispatcher
    func_001AFC10(self, ex, arg3);
}
