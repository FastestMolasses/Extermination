// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Resets the input/aim state block when func_001B5F40 reports inactive (==0):
// six s16 fields (D_00810E70..7A) cleared, two u8 axes (D_00810E64/65) recentered
// to 0x80. Matched via mwcc 2.3.3 (991202 fills the bnez delay slot with the
// store-address lui = wall #13; 2.3.3 leaves the nop).
extern int func_001B5F40(short *, int *);
extern int D_00810E40;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;
extern short D_00810E70, D_00810E72, D_00810E74, D_00810E76, D_00810E78, D_00810E7A;

void func_001B57E0(void) {
    if (func_001B5F40(&D_00810E70, &D_00810E40) == 0) {
        D_00810E70 = 0;
        D_00810E72 = 0;
        D_00810E74 = 0;
        D_00810E76 = 0;
        D_00810E78 = 0;
        D_00810E7A = 0;
        D_00810E64 = 0x80;
        D_00810E65 = 0x80;
    }
}
