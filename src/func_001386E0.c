// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame update for an actor (self) against its owning
// entity/context record (ent).  Runs only while func_001B2140(self) is true.
// Dispatches the actor sub-state byte self+0x05 through jtbl_0026D1C0
// (6 states -> func_00138900 / func_00138C20 / func_00139240 /
// func_001399F0 / func_00139E00 / func_0013A3B0, each called (self, ent)).
// Then, unconditionally:
//   - decrement-to-zero timers ent+0x86 (byte), ent+0x32 and ent+0x2E (half)
//   - ent+0x84 set  -> self+0x5D = 0x81 and func_001B4810(self)
//     otherwise     -> self+0x5D = 1
//   - if self+0x36 (signed half) != 0:
//       ent+0x34 != 0 || ent+0x83 != 0 -> self+0x00 = 1, self+0x36 = 0
//       else                           -> self+0x04 = 2, self+0x05 = 0,
//                                         self+0x06 = 0
//   - if ent+0x70 (low byte) == 8: func_001FBD50(self,
//       ((func_00122BB8() >> 17) % 5) + 0x821, 0, 300.0f)   -- a sound/effect
//       id picked from 5 variants.  func_00122BB8 is called with no argument
//       set-up in the original ($a0 still holds self), so it is declared
//       argument-less here.
//   - self+0x0A = 0, func_0013BF20(self, ent), func_0013BE60(self, ent),
//     ent+0x70 (full word) = anim_advance_time(self, 1.0f),
//     func_00131ED0/func_001C6910/func_001B17A0(self), then the virtual
//     method at self+0x4C.
// Note ent+0x70 is read as a byte but written as a word.
//
// func_001B2140 takes no register argument in its own body (it only reads
// globals), but the call site here is byte-identical ONLY if it is prototyped
// as taking one pointer: the original passes `self`, which is already in $a0,
// so no argument set-up is emitted and the two parameter saves come out as
// `paddub s0,a1` / `paddub s1,a0` (a1 first) instead of the reverse.

extern int func_001B2140(char *);
extern void func_00138900(char *, char *);
extern void func_00138C20(char *, char *);
extern void func_00139240(char *, char *);
extern void func_001399F0(char *, char *);
extern void func_00139E00(char *, char *);
extern void func_0013A3B0(char *, char *);
extern void func_001B4810(char *);
extern int func_00122BB8();
extern void func_001FBD50(char *, int, int, float);
extern void func_0013BF20(char *, char *);
extern void func_0013BE60(char *, char *);
extern int anim_advance_time(char *, float);
extern void func_00131ED0(char *);
extern void func_001C6910(char *);
extern void func_001B17A0(char *);

void func_001386E0(char *self, char *ent) {
    int t;

    if (func_001B2140(self) != 0) {
        switch (*(unsigned char *)(self + 5)) {
        case 0:
            func_00138900(self, ent);
            break;
        case 1:
            func_00138C20(self, ent);
            break;
        case 2:
            func_00139240(self, ent);
            break;
        case 3:
            func_001399F0(self, ent);
            break;
        case 4:
            func_00139E00(self, ent);
            break;
        case 5:
            func_0013A3B0(self, ent);
            break;
        }

        if (*(char *)(ent + 0x86) != 0) {
            *(char *)(ent + 0x86) = *(char *)(ent + 0x86) - 1;
        }
        if (*(unsigned short *)(ent + 0x32) != 0) {
            *(unsigned short *)(ent + 0x32) = *(unsigned short *)(ent + 0x32) - 1;
        }
        if (*(unsigned short *)(ent + 0x2E) != 0) {
            *(unsigned short *)(ent + 0x2E) = *(unsigned short *)(ent + 0x2E) - 1;
        }

        if (*(char *)(ent + 0x84) != 0) {
            *(unsigned char *)(self + 0x5D) = 0x81;
            func_001B4810(self);
        } else {
            *(char *)(self + 0x5D) = 1;
        }

        if (*(short *)(self + 0x36) != 0) {
            if (*(unsigned short *)(ent + 0x34) != 0 || *(char *)(ent + 0x83) != 0) {
                *(char *)(self + 0) = 1;
                *(short *)(self + 0x36) = 0;
            } else {
                *(char *)(self + 4) = 2;
                *(char *)(self + 5) = 0;
                *(char *)(self + 6) = 0;
            }
        }

        if (*(unsigned char *)(ent + 0x70) == 8) {
            t = func_00122BB8() >> 17;
            func_001FBD50(self, (t % 5) + 0x821, 0, 300.0f);
        }

        *(char *)(self + 0xA) = 0;
        func_0013BF20(self, ent);
        func_0013BE60(self, ent);
        *(int *)(ent + 0x70) = anim_advance_time(self, 1.0f);
        func_00131ED0(self);
        func_001C6910(self);
        func_001B17A0(self);
        (*(void (**)(char *))(self + 0x4C))(self);
    }
}
