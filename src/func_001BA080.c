// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS:
// Script opcode 0x06 handler (ftab_0024D880[0x06]; the script interpreter is func_001BA1F0).
// Flag/counter ops over two parallel byte arrays indexed by the script record's slot index:
//   D_00810758[] — event flags,  D_008107D8[] — script counters.
// The record is the THIRD parameter ($a2); params 1 and 2 are dead here (mwcc reuses $a0 as a
// constant scratch — the "constant parked in the dead first param" idiom).
//   rec[2] (+0x08) = sub-opcode,  rec[5] (+0x14) = slot index,  rec[6] (+0x18) = operand.
// Sub-op:
//   0 -> flag[slot]    = 1
//   1 -> flag[slot]    = 0xFF
//   2 -> WAIT until counter[slot] != 0          (returns 0 = "not done" while it is 0)
//   3 -> counter[slot] = (unsigned char)operand  (BYTE read of +0x18)
//   4 -> WAIT until counter[slot] == operand     (WORD read of +0x18)
//   5 -> counter[slot]++
//   6 -> counter[slot]--
//   >=6 (out of range) -> 1
// Return 1 = opcode complete / advance the script; 0 = stay on this opcode next tick.
// Note the deliberate width asymmetry on +0x18: case 3 loads a byte (lbu), case 4 loads a word (lw).

extern unsigned char D_00810758[];
extern unsigned char D_008107D8[];

int func_001BA080(int op, int arg1, int *rec)
{
    int ret;

    switch (rec[2]) {
    case 0:
        op = 1;
        D_00810758[rec[5]] = op;
        break;
    case 1:
        op = 0xFF;
        D_00810758[rec[5]] = op;
        break;
    case 2:
        if (D_008107D8[rec[5]] != 0) {
            break;
        }
        ret = 0;
        goto out;
    case 3:
        D_008107D8[rec[5]] = *((unsigned char *)rec + 0x18);
        break;
    case 4:
        if (D_008107D8[rec[5]] == rec[6]) {
            break;
        }
        ret = 0;
        goto out;
    case 5:
        D_008107D8[rec[5]]++;
        break;
    case 6:
        D_008107D8[rec[5]]--;
    }
    ret = 1;
out:
    return ret;
}
