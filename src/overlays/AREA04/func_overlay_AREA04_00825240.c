// NEARMISS
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA04 overlay: TWO real functions in the splat slot 00825240 (pieces
// 00825240 and 00825280). overlay_match.py true_functions groups them as one
// 0x290-byte function because splat has no piece at the second function's
// start (link 0x008252D0, runtime 0x00825310; nothing calls it directly).
//   func_overlay_AREA04_00825240: runtime 0x00825280, 0x8C bytes, leaf.
//   func_overlay_AREA04_008252D0: runtime 0x00825310, 0x200 bytes.
// Both compile byte-identical (per-function .text sections checked one by
// one against the original bytes, lane A04C). The NEARMISS marker is for a
// link-tool reason only: mwcc 2.3.3 emits one .text section per function,
// and fill_overlay.py (_obj_text_size / plan_absorption) reads only the
// first .text of an object, so this object would fail the absorption
// check. The link assembles the two splat pieces instead; the overlay stays
// byte-identical. See docs/AREA04_OVERLAY.md.
// Role 00825240 (called as 0x825280 from 0x825310): returns 1 when
//  D_00810350 < 552 or D_00810358 > 279 or D_00810358 < 235, else 0.
// Role 008252D0: state 0 sets state 1 and +0 = 1. State 1, sub-state +5:
//  0 when func_001BA1C0(self, 0x31) starts one of three scripts once each
//  (flag bits in D_00810821): 0x829570 for D_00810702 == 0 and 0x829930
//  for D_00810702 == 2 (both only while D_70003B8D != 4), 0x829CF0 for
//  D_00810702 == 7 when 0x825280 returns 1; each sets sub 1 and returns.
//  1 goes to state 3 at script end. State 1 ends with func_001B17A0;
//  states 2/3 func_001AFC10.
extern float D_00810350;
extern float D_00810358;
extern unsigned char D_00810702;
extern unsigned char D_70003B8D;
extern unsigned char D_00810821;
extern char D_overlay_AREA04_00829570[];
extern char D_overlay_AREA04_00829930[];
extern char D_overlay_AREA04_00829CF0[];
extern int func_001BA1C0(unsigned char *self, int n);
extern void func_001BA1A0(void *who, void *script);
extern int func_001BA1F0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern int func_overlay_AREA04_00825280(void);

int func_overlay_AREA04_00825240(void) {
    if (D_00810350 < 552.0f) {
        return 1;
    }
    if (D_00810358 > 279.0f) {
        return 1;
    }
    if (D_00810358 < 235.0f) {
        return 1;
    }
    return 0;
}

void func_overlay_AREA04_008252D0(unsigned char *self) {
    unsigned char *blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        self[4] = 1;
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 0x31) != 0) {
                if (D_00810702 == 0 && D_70003B8D != 4 && !(D_00810821 & 1)) {
                    func_001BA1A0(blk, D_overlay_AREA04_00829570);
                    self[5] = 1;
                    D_00810821 |= 1;
                    return;
                }
                if (D_00810702 == 2 && D_70003B8D != 4 && !(D_00810821 & 2)) {
                    func_001BA1A0(blk, D_overlay_AREA04_00829930);
                    self[5] = 1;
                    D_00810821 |= 2;
                    return;
                }
                if (D_00810702 == 7 && func_overlay_AREA04_00825280() != 0 && !(D_00810821 & 4)) {
                    func_001BA1A0(blk, D_overlay_AREA04_00829CF0);
                    self[5] = 1;
                    D_00810821 |= 4;
                    return;
                }
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[4] = 3;
            }
            break;
        }
        func_001B17A0(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
