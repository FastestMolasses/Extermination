// COMPILER: eegcc
// CFLAGS: -O2
//
// MATCH NOTE (m3-matching lane, 72.5% -> 100%): ee-gcc (-G0) addresses
// D_0024295C with lui/lw as the target does; mwcc under the global flags made
// it gp-relative. Seed store for func_00122BB8's LCG state at +0x58.

struct rand_state_block { char pad[0x58]; unsigned int next; };
extern struct rand_state_block *D_0024295C;

void func_00122BA8(unsigned int seed) {
    D_0024295C->next = seed;
}
