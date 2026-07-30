// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: VU0 stop/force-break helper (Sony SDK / lowmem region).
// func_0011AB20 reads COP2 control register 29 (VPU_STAT) and returns a VU0
// run-state code decoded from the VU0 half of that register:
//   0 = idle, 1 = VSS0 (stopped on E-bit), 2 = VFS0 (stopped by force break),
//   3 = VIS0 (stopped by interrupt), 4 = VBS0 (running).
// This function returns 1 for states 0, 3 and 4, and 0 for states 1 and 2.
// In state 4 (VU0 still running) it first sets bit 0 (FBK0, VU0 force break)
// in COP2 control register 28 (FBRST) before returning 1.
// The sibling func_0011AEA0 is the identical routine for VU1 (FBRST bit 8).

extern int func_0011AB20(void);

int func_0011A9F0(void) {
    int ok;

    ok = 0;
    switch (func_0011AB20()) {
    case 0:
    case 3:
        ok = 1;
        break;
    case 1:
    case 2:
        break;
    case 4:
        /* FBRST |= 0x001  — force-break VU0 */
        __asm__ __volatile__(
            "cfc2.ni $2, $vi28\n"
            "\tori   $2, $2, 0x1\n"
            "\tctc2.ni $2, $vi28"
            ::: "$2");
        ok = 1;
        break;
    }
    return ok;
}
