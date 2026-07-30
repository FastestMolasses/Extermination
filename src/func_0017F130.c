// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Game-mode gated proximity/aim probe: in mode 4 it calls func_0017E6E0 with
// (0.0f, 1.0f); in mode 17 it first requires arg0's flag byte at +0x316 to be set,
// then calls func_0017E6E0 with (19.0f, 1.0f). Returns 1 when the probe reports
// non-zero, 0 otherwise (and 0 for every other mode).
// The `int oi = 1; float one = (float)oi;` staging is load-bearing: the int->float
// cast survives as an IR node and makes mwcc emit `mtc1 v0,$f13` BEFORE
// `mtc1 zero,$f12` (the inverse of idiom-24). The `else if` with a single trailing
// `return 0` (no `return 0` inside the mode-4 arm) is what keeps the mode-4 result
// as CW's beqz/b/li branch pair instead of collapsing it to a movz/movn select.
extern unsigned char D_00810700;
extern int func_0017E6E0(float yaw, float range);

int func_0017F130(char *arg0) {
    if (D_00810700 == 4) {
        int oi = 1;
        float one = (float)oi;
        if (func_0017E6E0(0.0f, one) != 0) {
            return 1;
        }
    } else if (D_00810700 == 0x11) {
        if (*(unsigned char *)(arg0 + 0x316) != 0) {
            if (func_0017E6E0(19.0f, 1.0f) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
