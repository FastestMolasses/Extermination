// NEARMISS func_0013D220  (vram 0x0013D220, 0xA4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Single branch delay-slot scheduling difference: target emits 'bnez v0,0x74; nop' (empty slot) after the func_0019B6C0 return test; mwcc 2.3.3 hoists the following block's 'lui at,0x7000' into the slot, shifting the branch target to 0x78. Not the clean-store nop; flag fixes/opt levels do not chang...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies arg0->{+0xB0,+0xB4,+0xB8} into the scratchpad vector at 0x700038E0,
// sets +0xC component to 1.0, subtracts 10.0 from the y component, calls
// func_0019B6C0(&arg0->+0xB0, &vec). Returns 1 only if that succeeds AND the
// object at *0x700031D0 has byte +0x1A == 0x5B, else 0. 0x700038E0 is a real
// symbol (passed by address via %hi/%lo) while the component stores and the
// 0x700031D0 access are raw absolute addresses (matches target).
// NEARMISS: o233 98.41%. All logic + content correct; sole residual is one
// branch delay-slot: target leaves 'bnez v0; nop' empty, mwcc 2.3.3 hoists the
// next block's 'lui at' into the slot (branch target shifts 0x74->0x78).
// Delay-slot scheduling artifact, not the clean-store nop.
extern int func_0019B6C0(float *, float *);
extern float D_700038E0;

int func_0013D220(unsigned char *arg0) {
    *(float *)0x700038E0 = *(float *)(arg0 + 0xB0);
    *(float *)0x700038E4 = *(float *)(arg0 + 0xB4);
    *(float *)0x700038E8 = *(float *)(arg0 + 0xB8);
    *(int *)0x700038EC = 0x3F800000;
    *(float *)0x700038E4 = *(float *)0x700038E4 - 10.0f;
    if (func_0019B6C0((float *)(arg0 + 0xB0), &D_700038E0) == 0) {
        return 0;
    }
    if (*(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x5B) {
        return 1;
    }
    return 0;
}
