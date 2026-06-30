// NEARMISS func_001A8840  (vram 0x001A8840, 0x130 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.24% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-delay-slot scheduling artifact: both mwcc 2.3.3 and 991202 hoist the trailing *(short*)0x70003B86 store's address-load (lui at,0x7000) into the inner beqz delay slot; original CW 2.3.1 leaves the slot nop and materializes the address at the merge. Single-instruction difference, inverse of ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Proximity/collision check between entity arg0 and entity arg1. Using
// func_0011DF78 (fabs), tests |arg0.x(0xA0)-arg1.x(0xB0)| <= bounds[0],
// |arg0.z(0xA8)-arg1.z(0xB8)| <= bounds[2], |arg0.y(0xA4)-arg1.y(0xB4)| <=
// 1.5 + bounds[1], where bounds = *(arg1+0x30). On all-in-range, branches on the
// state byte arg1+0xD: state 0 -> sets arg1+0xA=1, fires func_00187EC0(6, arg1+0x56),
// and if (arg1+0xB && D_00810707!=1 && arg0[0]==1) sets arg0+0x22C=5.0f bits and
// arg0[0]=3; state 1 -> func_00187EC0(7, 0). Always clears the short at 0x70003B86.
//
// NEARMISS (best 97.24% on mwcc 2.3.3, 93.29% on 991202). Logic fully recovered;
// everything is byte-identical EXCEPT one instruction: the trailing store
// *(short*)0x70003B86 = 0 has its address-load (lui at,0x7000) hoisted by mwcc
// into the delay slot of the inner `beqz` (arg1+0xB == 0 test), shifting the
// branch target by one. The original CW 2.3.1 leaves that delay slot a nop and
// materializes the store address only at the merge point. Single branch-delay-slot
// scheduling artifact; both mwcc builds hoist, 2.3.3 does not undo it (it is the
// inverse of the clean-store-nop case 2.3.3 cracks). The lbu-vs-lb narrowing on
// arg1+0x56 was fixed by typing func_00187EC0's 2nd param `unsigned char`.
extern float func_0011DF78(float a);
extern void func_00187EC0(char a0, unsigned char a1);
extern unsigned char D_00810707;

void func_001A8840(char *arg0, char *arg1) {
    unsigned char t;

    if (func_0011DF78(*(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0)) <= **(float **)(arg1 + 0x30) &&
        func_0011DF78(*(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8)) <= *(float *)(*(char **)(arg1 + 0x30) + 8) &&
        func_0011DF78(*(float *)(arg0 + 0xA4) - *(float *)(arg1 + 0xB4)) <= 1.5f + *(float *)(*(char **)(arg1 + 0x30) + 4)) {
        t = *(unsigned char *)(arg1 + 0xD);
        if (t == 0) {
            *(char *)(arg1 + 0xA) = 1;
            func_00187EC0(6, *(unsigned char *)(arg1 + 0x56));
            if (*(unsigned char *)(arg1 + 0xB) != 0 && D_00810707 != 1 && *(unsigned char *)arg0 == 1) {
                *(int *)(arg0 + 0x22C) = 0x40A00000;
                *(char *)arg0 = 3;
            }
        } else if (t == 1) {
            func_00187EC0(7, 0);
        }
        *(short *)0x70003B86 = 0;
    }
}
