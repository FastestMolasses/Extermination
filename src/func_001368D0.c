// NEARMISS func_001368D0  (vram 0x001368D0, 0x36C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Two residual artifacts, both scheduling/coloring not logic: (1) the first early-return guard compiles to mirror-image branch polarity/target vs CW's short-hop `bnez` in every source shape tried; (2) the `(short)delta >= field` compare keeps its slt result in $at in target vs GPR in mwcc (same $at...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-entity hit/damage-over-time tick (arg0 = entity, arg1 = attacker/DoT
// source). If arg0+0x36's pending-hit word is nonzero and arg1 isn't already
// mid-hit (arg1+0x52==0), consumes it: marks arg0 state=2, extracts a 12-bit
// signed delta from arg0+0x36 (scaled x5 if bit 0x8000 set), accumulates it
// into arg1+0x54 (or resets), sets arg1+0x58=0x19, and — if the "flinch" flag
// 0x4000 is set and not already flinching — starts a flinch timer via
// func_001EFE00(0x80000027, arg0). Subtracts the delta from the HP-like
// counter arg0+0x34 (clamped to 0, triggering func_00136F20(arg0,0) death/
// reset on depletion). Then, for each of 4 status-effect bits in arg1+0x5B
// (0x10/2/4/1) gated on arg0+0x34 falling below a threshold (0x191/0x12D/0x97
// /0x33), clears the bit (partially), sets the "flinch" bit 0x8000 on
// arg0+0x36, and fires the matching cue: bit 0x10 builds a GS packet at
// 0x700038A0 and calls func_001EFD90(0x80000058,...); bits 2/4/1 call
// func_001EFE00(id,arg0) + func_00136F20(arg0, slot). Finally, if HP is still
// nonzero, arbitrates the next hit-reaction animation via arg1+0x54/arg0+0x36
// flags (returns 0, "still alive"); if HP hit 0, resets arg0's state bytes
// 4/5/6 and arg1+0x54, returning 1 ("died").
extern void func_001EFE00(int a, char *p);
extern void func_00136F20(int p, int sel);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void copy_qw4(void *a0, void *a1);
extern void func_001EFD90(int a0, void *a1, void *a2);
extern char *D_00275B40;
extern char D_700036A0[];
extern char D_700038A0[];

int func_001368D0(char *arg0, char *arg1) {
    short flags;
    int delta;

    if (*(short *)(arg0 + 0x36) == 0) {
        goto skip;
    }
    if (*(short *)(arg1 + 0x52) != 0) {
        *(short *)(arg0 + 0x36) = 0;
        goto skip;
    }
    *(char *)(arg0 + 0) = 2;
    *(short *)(arg1 + 0x52) = 0xFF;

    flags = *(short *)(arg0 + 0x36);
    delta = (short)(flags & 0xFFF);
    if (flags & 0x8000) {
        delta = (short)(delta * 5);
    }
    if (*(char *)(arg1 + 0x58) != 0) {
        *(short *)(arg1 + 0x54) = (short)(*(short *)(arg1 + 0x54) + delta);
    } else {
        *(short *)(arg1 + 0x54) = (short)delta;
    }
    *(char *)(arg1 + 0x58) = 0x19;

    if (*(char *)(arg1 + 0x5E) == 0 && (*(short *)(arg0 + 0x36) & 0x4000)) {
        *(char *)(arg1 + 0x5E) = 0x3C;
        func_001EFE00(0x80000027, arg0);
    }

    if ((short)delta >= *(short *)(arg0 + 0x34)) {
        *(short *)(arg0 + 0x34) = 0;
        func_00136F20((int)arg0, 0);
    } else {
        *(short *)(arg0 + 0x34) = (short)(*(short *)(arg0 + 0x34) - delta);
    }

    {
        signed char f5B = *(signed char *)(arg1 + 0x5B);
        if ((f5B & 0x10) && *(short *)(arg0 + 0x34) < 0x191) {
            *(signed char *)(arg1 + 0x5B) = f5B & 0xF;
            *(short *)(arg0 + 0x36) |= 0x8000;
            *(int *)0x700038A0 = 0x40000000;
            *(int *)0x700038A4 = 0x40880000;
            *(int *)0x700038A8 = 0;
            *(int *)0x700038AC = 0x3F800000;
            copy_qw4(&D_700036A0, *(char **)(D_00275B40 + 0x6C) + 0x90);
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            *(short *)(*(char **)(D_00275B40 + 0x6C) + 0x88) = 0;
            *(short *)(*(char **)(D_00275B40 + 0x6C) + 0x8A) = 0;
            *(short *)(*(char **)(D_00275B40 + 0x6C) + 0x8C) = 0;
            func_001EFD90(0x80000058, &D_700038A0, arg0 + 0xC0);
        }
    }

    {
        signed char f5B = *(signed char *)(arg1 + 0x5B);
        if ((f5B & 2) && *(short *)(arg0 + 0x34) < 0x12D) {
            *(signed char *)(arg1 + 0x5B) = f5B & 0x1D;
            *(short *)(arg0 + 0x36) |= 0x8000;
            func_001EFE00(0x80000055, arg0);
            func_00136F20((int)arg0, 2);
        }
    }

    {
        signed char f5B = *(signed char *)(arg1 + 0x5B);
        if ((f5B & 4) && *(short *)(arg0 + 0x34) < 0x97) {
            *(signed char *)(arg1 + 0x5B) = f5B & 0x1B;
            *(short *)(arg0 + 0x36) |= 0x8000;
            func_001EFE00(0x80000057, arg0);
            func_00136F20((int)arg0, 3);
        }
    }

    {
        signed char f5B = *(signed char *)(arg1 + 0x5B);
        if ((f5B & 1) && *(short *)(arg0 + 0x34) < 0x33) {
            *(signed char *)(arg1 + 0x5B) = f5B & 0x1E;
            *(short *)(arg0 + 0x36) |= 0x8000;
            func_001EFE00(0x80000056, arg0);
            func_00136F20((int)arg0, 1);
        }
    }

    if (*(short *)(arg0 + 0x34) != 0) {
        short st = *(short *)(arg0 + 0x36);
        if (*(short *)(arg1 + 0x54) >= 0x19 || (st & 0xA000) != 0) {
            goto done;
        }
        if (st & 0x5000) {
            *(short *)(arg1 + 0x52) = 0x1E;
        } else {
            *(short *)(arg1 + 0x52) = 0;
        }
        *(char *)(arg0 + 0) = 1;
        *(short *)(arg0 + 0x36) = 0;
skip:
        return 0;
    }
done:
    *(char *)(arg0 + 4) = 2;
    *(char *)(arg0 + 5) = 0;
    *(char *)(arg0 + 6) = 0;
    *(short *)(arg1 + 0x54) = 0;
    return 1;
}
