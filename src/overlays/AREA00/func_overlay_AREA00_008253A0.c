// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x008253E0 (splat/link name 008253A0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: two-step counter on the record in the second argument: +4 0
//  resets +5; each call increments +5, plays sound 0x19A every 16 counts and
//  returns 1 once +5 exceeds 48 (else 0).
extern void func_001FB9F0(int id, int a1, int a2, int a3);

int func_overlay_AREA00_008253A0(int unused, unsigned char *s) {
    switch (s[4]) {
    case 0:
        s[4]++;
        s[5] = 0;
    case 1:
        s[5]++;
        if ((s[5] & 0xF) == 0) {
            func_001FB9F0(0x19A, 0x1000, 0x1000, 0x1000);
        }
        if (s[5] > 0x30) return 1; break;
    }
    return 0;
}
