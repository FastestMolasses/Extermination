// COMPILER: eegcc
// CFLAGS: -O2
//
// SEMANTICS: VU1 stop / force-break helper (Sony SDK / lowmem region).
// func_0011AFD0 reads COP2 control register 29 (VPU_STAT) and returns the VU1
// run-state decoded from the VU1 half of that register:
//   0 = idle, 1 = VSS1 (stopped on E-bit), 2 = VFS1 (stopped by force break),
//   3 = VIS1 (stopped by interrupt), 4 = VBS1 (running).
// Returns 1 for states 0, 3 and 4, and 0 for states 1 and 2. In state 4 (VU1
// still running) it first sets bit 8 (FBK1, VU1 force break) in COP2 control
// register 28 (FBRST) before returning 1.
// The sibling func_0011A9F0 is the identical routine for VU0 (FBRST bit 0).

extern int func_0011AFD0(void);

int func_0011AEA0(void) {
    int ok;

    ok = 0;
    switch (func_0011AFD0()) {
    case 0:
    case 3:
        ok = 1;
        break;
    case 1:
    case 2:
        break;
    case 4:
        /* FBRST |= 0x100  — force-break VU1 */
        __asm__ __volatile__(
            "cfc2.ni $2, $vi28\n"
            "\tori   $2, $2, 0x100\n"
            "\tctc2.ni $2, $vi28"
            ::: "$2");
        ok = 1;
        break;
    }
    return ok;
}
