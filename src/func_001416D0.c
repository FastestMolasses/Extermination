// NEARMISS func_001416D0  (vram 0x001416D0, 0x214 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc list-scheduling artifact (NOT clean-store nop): the float-copy block interleaves lwc1/swc1 pairs and hoists the shared 1.0f int constant-load into an early slot, and the callee-saved arg-register copies (paddub $s0/$s1) sit in the func_001B2B10 jal delay slot. Body/logic fully correct. 233 b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 80.4%, 991202: 74.9%). Seeds a scratch vector at 0x700038A0
// (0, 3.0f, 13.0f, 1.0f), runs func_001B2B10 then func_001028B8 (target arg0+0xB0),
// then func_001B2BF0(self, &D_700038A0, &D_700038D0, PI/2) which returns a bitmask
// in `flags`. Depending on flags bit0 and bit4 it either builds an 8-word
// transform on the stack from a global entity (*0x700031D0)+0x24/0x28/0x2C and
// the vector at 0x700031B0/B4/B8, plus two 1.0f words, and calls
// func_001B3670(self, &sp[0], &sp[4]) storing the byte result at arg1+0x83;
// or (bit4 clear) sets arg1+0x83 = 2 and stores func_001B37D0(self,10.0f,20.0f)
// at arg1+0x44. When flags bit0 set and bit4 clear it gates on
// (self->0xB4 - 20.0f) < *0x700038D0, early-returning 0.
//
// WALL: body/logic fully recovered. Residual is pure mwcc list-scheduling:
// the float-copy block interleaves its lwc1/swc1 pairs and hoists the shared
// 1.0f int constant-load (`lui`) into an early slot, and the callee-saved
// arg-register copies (paddub $s0/$s1) are placed in the func_001B2B10 jal delay
// slot. Reordering the C assignments (24/28/2C before B0/B4/B8) recovers part of
// it (78.4 -> 80.4) but the exact load interleave is scheduler-internal. Not the
// clean-store nop; 233 does not close it. Parked as readable near-miss.
extern void func_001B2B10(int *, int *);
extern void func_001028B8(int *, int *, int);
extern int func_001B2BF0(char *, int *, int *, float);
extern char func_001B3670(char *, float *, float *);
extern float func_001B37D0(char *, float, float);
extern int D_700038A0;
extern int D_700038D0;

int func_001416D0(char *arg0, char *arg1) {
    int sp[8];
    int flags;
    char *p;

    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x40400000;
    *(int *)0x700038A8 = 0x41500000;
    *(int *)0x700038AC = 0x3F800000;
    func_001B2B10(&D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, &D_700038A0, (int)(arg0 + 0xB0));
    flags = func_001B2BF0(arg0, &D_700038A0, &D_700038D0, 3.14159274f);
    if (!(flags & 1)) {
        if (flags & 4) {
            p = *(char **)0x700031D0;
            *(float *)((char *)sp + 0x00) = *(float *)(p + 0x24);
            *(float *)((char *)sp + 0x04) = *(float *)(p + 0x28);
            *(float *)((char *)sp + 0x08) = *(float *)(p + 0x2C);
            *(float *)((char *)sp + 0x10) = *(float *)0x700031B0;
            *(float *)((char *)sp + 0x14) = *(float *)0x700031B4;
            *(float *)((char *)sp + 0x18) = *(float *)0x700031B8;
            *(int *)((char *)sp + 0x0C) = 0x3F800000;
            *(int *)((char *)sp + 0x1C) = 0x3F800000;
            *(char *)(arg1 + 0x83) = func_001B3670(arg0, (float *)sp, (float *)((char *)sp + 0x10));
        } else {
            *(char *)(arg1 + 0x83) = 2;
            *(float *)(arg1 + 0x44) = func_001B37D0(arg0, 10.0f, 20.0f);
        }
        return 1;
    }
    if (flags & 4) {
        p = *(char **)0x700031D0;
        *(float *)((char *)sp + 0x00) = *(float *)(p + 0x24);
        *(float *)((char *)sp + 0x04) = *(float *)(p + 0x28);
        *(float *)((char *)sp + 0x08) = *(float *)(p + 0x2C);
        *(float *)((char *)sp + 0x10) = *(float *)0x700031B0;
        *(float *)((char *)sp + 0x14) = *(float *)0x700031B4;
        *(float *)((char *)sp + 0x18) = *(float *)0x700031B8;
        *(int *)((char *)sp + 0x0C) = 0x3F800000;
        *(int *)((char *)sp + 0x1C) = 0x3F800000;
        *(char *)(arg1 + 0x83) = func_001B3670(arg0, (float *)sp, (float *)((char *)sp + 0x10));
        return 1;
    }
    if ((*(float *)(arg0 + 0xB4) - 20.0f) < *(float *)0x700038D0) {
        return 0;
    }
    *(char *)(arg1 + 0x83) = 2;
    *(float *)(arg1 + 0x44) = func_001B37D0(arg0, 10.0f, 20.0f);
    return 1;
}
