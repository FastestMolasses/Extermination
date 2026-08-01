// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: memory-card save step that writes the 0x640-byte SAVE-DATA block
// ("BASCUS-97112DS00-00.EX/DATA00", slot descriptor D_00267060[slot * 0x40]).
//
// `e` is the save-task record shared by the sub_BASCUS_97112_DS00_00_EX_*
// family:
//   e[+0x0A]           = save-slot index; selects the 64-byte card descriptor
//   e[+0x48], e[+0x4C] = card port / slot
//   e[+0x50], e[+0x54] = the mc-sync command / result cell pair
//   e[+0x64]           = the async sub-state word driven by func_001FE6B0
//
// 1. func_001FE9A0 polls the outstanding memory-card call (sceMcSync-style,
//    mode 1 = non-blocking) and also latches the result into D_00275C64.
//    Zero means "still busy" -> return 0 and retry on the next tick.
// 2. func_001AF220 refreshes the live save block in scratchpad, then
//    block_copy pulls the 0x640 bytes from scratchpad 0x70000000 into the
//    staging buffer D_00822690 that is handed to the card.
// 3. func_001FE6B0 runs the async write and reports 1 = complete, 0 = busy,
//    -1 = error.
//
// Return value: 1 when the write completed, 2 on error, and otherwise the
// shared abort flag D_00275C58 decides -- set means "fail with 2", clear means
// "still working, 0". The case-0 arm and the default arm share that tail, which
// is why the compiler emits the D_00275C58 load twice (once at the case-0 label
// and once rematerialised in the default arm's branch delay slot).
extern int func_001FE9A0(int, unsigned char *, unsigned char *);
extern void func_001AF220(void);
extern void block_copy(unsigned char *, unsigned char *, int);
extern int func_001FE6B0(unsigned char *, int, int, unsigned char *, unsigned char *, int);

extern unsigned char D_00267060[];
extern unsigned char D_00822690[];
extern int D_00275C58;

int sub_BASCUS_97112_DS00_00_EX_DATA_00_4(unsigned char *e)
{
    if (func_001FE9A0(1, e + 0x50, e + 0x54) == 0) {
        return 0;
    }
    func_001AF220();
    block_copy(D_00822690, (unsigned char *)0x70000000, 0x640);
    switch (func_001FE6B0(e + 0x64, ((int *)e)[0x12], ((int *)e)[0x13],
                          &D_00267060[e[0xA] << 6], D_00822690, 0x640)) {
    case -1:
        return 2;
    case 0:
        break;
    case 1:
        return 1;
    }
    if (D_00275C58 != 0) {
        return 2;
    }
    return 0;
}
