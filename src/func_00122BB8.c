// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane, 43.75% -> 100%): this is SDK-region code
// built with ee-gcc, not mwcc. The target's shape proves it: `lui v0` for the
// %hi then `v0` reused for 0x7FFFFFFF, and the store sunk into the `jr ra`
// delay slot; mwcc instead used $at and a dsll32/dsrl32 mask.
//
// Semantics (from the target at 0x00122BB8): the word at +0x58 of the block
// that D_0024295C points to is a linear-congruential state; it becomes
// state * 0x41C64E6D + 0x3039, is stored back, and the new state masked with
// 0x7FFFFFFF is returned. func_00122BA8 is the matching seed store.

struct rand_state_block { char pad[0x58]; unsigned int next; };
extern struct rand_state_block *D_0024295C;

int func_00122BB8(void) {
    struct rand_state_block *r = D_0024295C;
    r->next = r->next * 1103515245 + 12345;
    return r->next & 0x7fffffff;
}
