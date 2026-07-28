// NEARMISS func_001F6640  (vram 0x001F6640, 0xA4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.73% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-arg emit-order artifact, exactly the committed func_00137C80-class wall (99.84% parked there): body is byte-identical (all 41 words match incl. prologue, bnez/b entry shape, bnel+advance likely-slot, sw a3 jal-slot) EXCEPT one 3-instruction rotation: target emits 'mtc1 zero,f13' (trailing 0.0f...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Walks a table of 0x28-byte entries at p until a negative id (short at +0x00).
// For each entry whose handle at +0x24 is -1 (unbound), builds a float[4]
// position {+0xC, +0x10, +0x14, 1.0f} on the stack and binds the entry via
// func_001D7FA0(vec, D_0026EB70 + idx*0x10, 1, 1.0f, 0.0f), where idx is the
// short at +0x4; stores the returned handle back at +0x24.
extern char D_0026EB70[];
extern int func_001D7FA0(float *, char *, int, float, float);

void func_001F6640(char *p)
{
    float vec[4];
    int idx;

    if (p == 0) {
        return;
    }
    while (*(short *)p >= 0) {
        if (*(int *)(p + 0x24) == -1) {
            idx = *(short *)(p + 4);
            vec[0] = *(float *)(p + 0xC);
            vec[1] = *(float *)(p + 0x10);
            vec[2] = *(float *)(p + 0x14);
            vec[3] = 1.0f;
            *(int *)(p + 0x24) = func_001D7FA0(vec, D_0026EB70 + (idx << 4), 1, 1.0f, 0.0f);
        }
        p += 0x28;
    }
}
