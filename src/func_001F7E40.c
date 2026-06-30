// NEARMISS func_001F7E40  (vram 0x001F7E40, 0x508 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 55.17% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling/regalloc divergence across the 32-iter inner loop: target keeps the running accumulator in a single reg (s2, frame -0x50, s0-s3) and threads it read-before-update through ~7 float_to_int calls; mwcc inflates to s0-s4 + callee-saved f20 and reorders the body. Not a single id...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS ~55% (mwcc 2.3.3, marginally > 991202). Logic FULLY recovered and
// verified against the asm, but the 32-iteration inner loop schedules very
// differently: the target threads a single running PRNG-style accumulator `n`
// (n = n*37+0xB) read-before-update through ~7 float_to_int calls, keeping it in
// s2 with i in s1 (frame -0x50, regs s0-s3). mwcc here inflates to s0-s4 + a
// callee-saved f20 and reorders the whole body, so the percentage is dominated
// by instruction-scheduling/regalloc divergence rather than any single idiom.
// Returned as readable port ground-truth. Effect: state in *(u8*)(p+4); case 0
// seeds the accumulator from func_00122BB8 and a 60-frame timer at (p+0x1F4);
// case 1 runs 32 particle/quad emits (float_to_int -> 0x700038A0.. coords,
// func_001F7CF0) ramping a 0..1 fade at (p+0x1F8) by 1/180 per frame, then to
// state 3; cases 2/3 hand off to func_001AFC10.

extern int float_to_int(float);
extern int func_00122BB8(unsigned char);
extern void func_001AFC10(char *);
extern void func_001F7CF0(int *, int *, int *);

void func_001F7E40(char *arg0) {
    int *s3;
    int n;
    int i;
    int a2, t1;
    float fp;

    s3 = (int *)(arg0 + 0x1F0);
    switch (*(unsigned char *)(arg0 + 4)) {
    case 0:
        s3[0] = func_00122BB8(*(unsigned char *)(arg0 + 4));
        s3[1] = 0x3C;
        *(float *)(arg0 + 0x1F8) = 0.0f;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        n = s3[0];
        for (i = 0; i < 0x20; i++) {
            *(int *)0x700038A0 = float_to_int(512.0f * ((float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f));
            n = n * 0x25 + 0xB;
            *(int *)0x700038A4 = float_to_int(224.0f * ((float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f));
            n = n * 0x25 + 0xB;
            *(int *)0x700038A0 = *(int *)0x700038A0 + 0x700;
            *(int *)0x700038A4 = *(int *)0x700038A4 + 0x790;
            *(int *)0x700038B0 = float_to_int(320.0f + 128.0f * ((float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f));
            n = n * 0x25 + 0xB;
            *(int *)0x700038B4 = float_to_int(160.0f + 64.0f * ((float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f));
            if ((n >> 0x10) & 0xFFFF & 1) {
                *(float *)0x70003A20 = -192.0f;
            } else {
                *(float *)0x70003A20 = 192.0f;
            }
            fp = *(float *)0x70003A20 * (*(float *)(arg0 + 0x1F8) * (0.7f + 0.3f * ((float)(((n * 0x25 + 0xB) >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f)));
            *(float *)0x70003A20 = fp;
            *(int *)0x700038A0 = float_to_int((float)*(int *)0x700038A0 + fp);
            a2 = *(int *)0x700038B0;
            *(int *)0x700038C0 = 0x80;
            *(int *)0x700038C4 = 0x80;
            *(int *)0x700038C8 = 0x80;
            *(int *)0x700038A0 = *(int *)0x700038A0 - (a2 >> 1);
            *(int *)0x700038A4 = *(int *)0x700038A4 - (*(int *)0x700038B4 >> 1);
            *(int *)0x700038B0 = a2 + *(int *)0x700038A0;
            t1 = *(int *)0x700038B4;
            *(int *)0x700038B4 = t1 + *(int *)0x700038A4;
            *(int *)0x700038CC = float_to_int(32.0f + 16.0f * ((float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f));
            n = n * 0x25 + 0xB;
            *(int *)0x700038CC = float_to_int((float)*(int *)0x700038CC * (1.0f - *(float *)(arg0 + 0x1F8)));
            func_001F7CF0((int *)0x700038A0, (int *)0x700038B0, (int *)0x700038C0);
            n = n * 0x25 + 0xB;
        }
        s3[1] = s3[1] - 1;
        if (s3[1] < 0) {
            *(float *)(arg0 + 0x1F8) = *(float *)(arg0 + 0x1F8) + 0.0055555557f;
            if (!(*(float *)(arg0 + 0x1F8) <= 1.0f)) {
                *(unsigned char *)(arg0 + 4) = 3;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        break;
    }
}
