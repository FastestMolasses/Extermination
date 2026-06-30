// NEARMISS func_001751A0  (vram 0x001751A0, 0x1E4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot-fill / instruction-scheduling artifact on a fully-recovered body. The target hoists the 0x70003A24 store's address `lui at,0x7000` above the r<=PI/4 c.le.s compare and drops the `swc1 f0` into the bc1f branch delay slot; mwcc 2.3.3 instead fills that slot with the next band's `lui v1,0...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera/aim-angle classifier. Early-out if the scratchpad gate byte at
// 0x70003B8D is set. Latches the global flag D_00810E57 into self[0x23F]; if it
// latched zero, store self[0x24C] = -1 and return. Otherwise convert two byte
// globals (D_00810E65, D_00810E64) to radians as PI * ((unsigned)b / 256.0),
// take func_0011DE90 (cos) of each into self[0x244]/self[0x248], then form a
// heading via func_0011E620 (atan2): r = atan2(-self[0x248], self[0x244]).
// Offset it by PI and the global bias D_008106A0, wrap through func_001B1470,
// subtract the current yaw self[0xC4], wrap again, and stash the result at the
// scratchpad float 0x70003A20. func_0011DF78 maps that to a magnitude r, then:
//   r <= PI/4        -> 0x70003A24 = r; self[0x24C] = 0
//   PI/4 < r < 3PI/4 -> self[0x24C] = (scratch 0x70003A20 < 0) ? 2 : 3
//   r >= 3PI/4       -> self[0x24C] = 1
//
// NEARMISS 97.3% (mwcc 2.3.3; 991202 92.7%). Logic fully recovered. The residual
// is a delay-slot-fill/scheduling artifact: the target hoists the 0x70003A24
// store's address `lui` above the PI/4 compare and drops the `swc1` into the
// branch delay slot, whereas mwcc fills that slot with the next band's constant
// load and emits the store in the taken block. Pure instruction scheduling.
extern float func_0011DE90(float a);
extern float func_0011E620(float a, float b);
extern float func_001B1470(float a);
extern float func_0011DF78(float a);
extern unsigned char D_00810E57;
extern unsigned char D_00810E64;
extern unsigned char D_00810E65;
extern float D_008106A0;

void func_001751A0(unsigned char *self) {
    float a64, a65;
    float r;

    if (*(volatile unsigned char *)0x70003B8D != 0) {
        return;
    }

    self[0x23F] = D_00810E57;
    if (self[0x23F] == 0) {
        *(int *)(self + 0x24C) = -1;
        return;
    }

    a65 = 3.14159265f * ((float)(unsigned int)D_00810E65 / 256.0f);
    a64 = 3.14159265f * ((float)(unsigned int)D_00810E64 / 256.0f);

    *(float *)(self + 0x244) = func_0011DE90(a64);
    *(float *)(self + 0x248) = func_0011DE90(a65);

    r = func_0011E620(-*(float *)(self + 0x248), *(float *)(self + 0x244));
    r = func_001B1470((3.14159265f + r) + D_008106A0);
    r = func_001B1470(r - *(float *)(self + 0xC4));
    *(volatile float *)0x70003A20 = r;
    r = func_0011DF78(r);

    if (r <= 0.785398185f) {
        *(float *)0x70003A24 = r;
        *(int *)(self + 0x24C) = 0;
        return;
    }
    if (r >= 2.35619449f) {
        *(int *)(self + 0x24C) = 1;
    } else {
        if (*(volatile float *)0x70003A20 < 0.0f) {
            *(int *)(self + 0x24C) = 2;
        } else {
            *(int *)(self + 0x24C) = 3;
        }
    }
}
