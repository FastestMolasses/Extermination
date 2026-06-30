// NEARMISS func_00147960  (vram 0x00147960, 0x1E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.89% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two coupled compiler artifacts on a fully-recovered body. (1) The 16-wide table-address index addend is register-colored/grouped differently (commutative addu order, v1 vs a0 -- semantically identical). (2) The inner case-2 dispatch comes out plain `beq` where the target uses branch-likely `beql`...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state behavior tick keyed on the state byte self[6] (3-way switch 0/1/2).
// State 0: snapshot the 0x56 timer short, zero arg1+0x50/+0x4C, then roll a
// random value (func_00122BB8) and index the 16-wide table D_00245B20 by
// row (timer!=0 ? 1 : 0) and ((rand>>7)&0xF). On the resulting clip code:
//   2 -> self[5]=1, self[6]=0 (hand off to next state)
//   1 -> self[6]=1, anim_clip_init(self, 1, 5.0f, 0.0f)
//   0 -> self[6]=1, set arg1+0x28 = (((rand>>18)&0x7F)+0x78) as u16, and if the
//        flag word at self+0x2C (masked ~0x8000) is set, anim_clip_init(self,0,5,0)
// State 1: decrement the arg1+0x28 counter; when it hits 0, clear self[6].
// State 2: if arg1+0x20 has bit 0x1000 set, clear self[6].
// Tail (all states): reload the 0x56 timer short. If nonzero, when arg1+0x68 <=
// (float)timer OR self[0xA]&1, clear the timer. If zero, run func_0014BB10 and,
// when arg1+0x38 != 0 OR self[0xA]&1, set self[5]=1, self[6]=0.
//
// NEARMISS 96.9% (mwcc 2.3.3; 991202 94.7%). Body/logic fully recovered; the
// residual is a compiler artifact: the table-address addend grouping is
// register-colored differently (commutative addu order, semantically identical)
// and the case-2 dispatch comes out `beq` where the target uses the branch-likely
// `beql` (whose delay slot preloads v1=1, which in turn shifts the case-1
// self[6]=1 store register and the f12/f13 move order). Pure scheduling/coloring.
extern void anim_clip_init(int self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_0014BB10(unsigned char *self, unsigned char *arg1);
extern unsigned char D_00245B20[];

void func_00147960(unsigned char *self, unsigned char *arg1) {
    short s;
    int r;
    int v;
    int b;

    switch (self[6]) {
    case 0:
        s = *(short *)(self + 0x56);
        *(int *)(arg1 + 0x50) = 0;
        r = (s != 0) ? 1 : 0;
        v = func_00122BB8();
        *(int *)(arg1 + 0x4C) = 0;
        b = D_00245B20[((v >> 7) & 0xF) + (r << 4)];
        switch (b) {
        case 0:
            self[6] = 1;
            v = func_00122BB8();
            *(int *)(arg1 + 0x28) = (unsigned short)(((v >> 18) & 0x7F) + 0x78);
            if ((*(short *)(self + 0x2C) & 0xFFFF7FFF) != 0) {
                anim_clip_init((int)self, 0, 5.0f, 0.0f);
            }
            break;
        case 1:
            self[6] = 1;
            anim_clip_init((int)self, 1, 5.0f, 0.0f);
            break;
        case 2:
            self[5] = 1;
            self[6] = 0;
            break;
        }
        break;
    case 1:
        v = *(int *)(arg1 + 0x28);
        if (v != 0) {
            *(int *)(arg1 + 0x28) = v - 1;
        } else {
            self[6] = 0;
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            self[6] = 0;
        }
        break;
    }

    s = *(short *)(self + 0x56);
    if (s != 0) {
        if (*(float *)(arg1 + 0x68) <= (float)s || (self[0xA] & 1)) {
            *(short *)(self + 0x56) = 0;
        }
    } else {
        func_0014BB10(self, arg1);
        if (*(int *)(arg1 + 0x38) != 0 || (self[0xA] & 1)) {
            self[5] = 1;
            self[6] = 0;
        }
    }
}
