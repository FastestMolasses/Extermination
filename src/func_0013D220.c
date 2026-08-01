// NEARMISS func_0013D220  (vram 0x0013D220, 0xA4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// NOT a build/allocator tie-break — a splat SYMBOLIZATION artifact in our own target object. Sole residual is the 'bnez v0' after func_0019B6C0: target leaves the delay slot EMPTY, mwcc233 peels the next block's 'lui $at,0x7000' (the *(int*)0x700031D0 deref) into the slot and retargets +4. Root cau...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies arg0->{+0xB0,+0xB4,+0xB8} into the scratchpad vector at 0x700038E0, sets the
// +0xC component to 1.0f, subtracts 10.0f from the y component, then calls
// func_0019B6C0(&arg0->+0xB0, &vec) — a room/broadphase probe. Returns 1 only if that
// probe succeeds AND the object published at *0x700031D0 has type byte +0x1A == 0x5B,
// else 0. 0x700038E0 is passed by address (%hi/%lo) while its component stores use the
// raw absolute form — match each site to what the target actually does.
extern int func_0019B6C0(float *, float *);
extern float D_700038E0;
extern char *D_700031D0;

int func_0013D220(unsigned char *arg0) {
    *(float *)0x700038E0 = *(float *)(arg0 + 0xB0);
    *(float *)0x700038E4 = *(float *)(arg0 + 0xB4);
    *(float *)0x700038E8 = *(float *)(arg0 + 0xB8);
    *(int *)0x700038EC = 0x3F800000;
    *(float *)0x700038E4 = *(float *)0x700038E4 - 10.0f;
    if (func_0019B6C0((float *)(arg0 + 0xB0), &D_700038E0) == 0) {
        return 0;
    }
    if (*(unsigned char *)(D_700031D0 + 0x1A) == 0x5B) {
        return 1;
    }
    return 0;
}
