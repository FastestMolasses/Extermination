// NEARMISS sub_BASCUS_97112_DS00_00_EX_DATA_00_2  (vram 0xASCUS_97112_DS00_00_EX_DATA_00_2, 0x1E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc list-scheduler + operand-order permutation (NOT the clean-store delay-slot nop, so 2.3.3 does not help — it is marginally worse at 96.4754). Body is otherwise byte-identical: 119 of 122 instructions match, all control flow (the descending 2-case dispatch on e[4], the beql with the %hi in its...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: two-state per-frame step for an overlay record `e`, driven by the
// state byte e[4]. Any state other than 0 or 1 does nothing.
//   e[+0x03] = completion/notify byte
//   e[+0x04] = state (0 = load in progress, 1 = finished -> reset)
//   e[+0x05] = secondary flag, cleared on reset
//   e[+0x0A] = slot index, selects both a 64-byte descriptor in D_00267060
//              and the destination 12-byte slot in this record
//   e[+0x48], +0x4C = the two 32-bit parameters handed to func_001FE6B0
//   e[+0x64] = 0x640-byte transfer/work area (also cleared on reset)
//   e[+0x6C + idx]        = per-slot "valid" byte
//   e[+0x74 + idx*12 ..]  = per-slot 12-byte record: bytes 0..5, byte 6, word 8
// State 0: refresh the staging record at D_00810700 (func_002284F0), then run
//   func_001FE6B0(e+0x64, e[0x48], e[0x4C], &D_00267060[idx*64], &D_00810700,
//   0x640). Its result is a 3-way status: 1 = complete, 0 and -1 = pending /
//   error (both just return). On 1 the destination slot is zeroed
//   (func_00121A28 = memset), marked valid, and filled from the staging record
//   (bytes 0..5 -> slot bytes 0..5, word at D_00810750 -> slot word 8, byte at
//   D_0081070A -> slot byte 6, in exactly that order), then the state advances
//   to 1.
// State 1: teardown -- e[3] = 3, state back to 0, e[5] = 0, work area cleared.
// The staging fields are referenced through separate D_008107xx externs (not
// one struct) because that is how the target's per-field %hi/%lo relocations
// are laid out.
//

extern void func_002284F0(unsigned char *);
extern int func_001FE6B0(unsigned char *, int, int, unsigned char *, unsigned char *, int);
extern void func_00121A28(unsigned char *, int, int);

extern unsigned char D_00267060[];
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810703;
extern unsigned char D_00810704;
extern unsigned char D_00810705;
extern unsigned char D_0081070A;
extern int D_00810750;

void sub_BASCUS_97112_DS00_00_EX_DATA_00_2(unsigned char *e)
{
    int r;

    switch (e[4]) {
    case 0:
        func_002284F0(&D_00810700);
        r = func_001FE6B0(e + 0x64, *(int *)(e + 0x48), *(int *)(e + 0x4C),
                          &D_00267060[e[0xA] << 6], &D_00810700, 0x640);
        switch (r) {
        case -1:
            break;
        case 0:
            break;
        case 1:
            func_00121A28(e + e[0xA] * 12 + 0x74, 0, 12);
            e[0x6C + e[0xA]] = 1;
            e[0x74 + e[0xA] * 12] = D_00810700;
            e[0x75 + e[0xA] * 12] = D_00810701;
            e[0x76 + e[0xA] * 12] = D_00810702;
            e[0x77 + e[0xA] * 12] = D_00810703;
            e[0x78 + e[0xA] * 12] = D_00810704;
            e[0x79 + e[0xA] * 12] = D_00810705;
            *(int *)(e + 0x7C + e[0xA] * 12) = D_00810750;
            e[0x7A + e[0xA] * 12] = D_0081070A;
            e[4] = 1;
            break;
        }
        break;
    case 1:
        e[3] = 3;
        e[4] = 0;
        e[5] = 0;
        *(int *)(e + 0x64) = 0;
        break;
    }
}
