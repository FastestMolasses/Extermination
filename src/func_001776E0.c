// NEARMISS func_001776E0  (vram 0x001776E0, 0x494 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 75.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction scheduling + tail lowering. mwcc hoists the 4.0f constant load before the scratch float-math chain and lowers `side?0:-1` with movz vs target's movn. Body byte-identical in structure; residual is a scheduling/regalloc permutation (permuter-class), not the clean-store nop and not jr-ta...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 75.6%, 991202: 68.9%). Logic fully recovered. Builds two
// AABB-style probe boxes in scratchpad blocks at 0x700038A0/B0/C0/D0 around a
// center derived from globals 0x70003050/3058 +/- 0.5*0x70003060/3068, runs
// func_0019AFE0 collision probes (mask 7) in two passes (offset +/-1 from the
// float arg f), accumulating a hit bitmask; if any pass hits it returns the
// mask. If neither does, two more probes (mask & 6) set a second bitmask and it
// returns 0 on a side-hit else -1.
//
// WALL: instruction scheduling + tail lowering. mwcc hoists the 4.0f (0x4080)
// constant load ahead of the scratch float-math chain (target schedules the
// A0/A8/A4 stores interleaved after the B0/B4/B8/BC stores), and lowers the
// final `side ? 0 : -1` with movz (target uses movn). Not the clean-store nop
// and not jr-table/VU0 — a scheduling/regalloc permutation; permuter territory.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_0019AFE0(int obj, void *a, void *b, int n);

extern char D_70003070[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

int func_001776E0(int obj, float f) {
    int hit = 0;
    int side = 0;

    *(float *)0x700038B0 = 4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 0.0f;
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038A0 = *(float *)0x70003050 - 0.5f * *(float *)0x70003060;
    *(float *)0x700038A8 = *(float *)0x70003058 - 0.5f * *(float *)0x70003068;
    *(float *)0x700038A4 = 1.0f + f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0(D_700038D0, D_70003070, D_700038B0);
    func_001028B8(D_700038B0, D_700038A0, D_700038D0);
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038C0 = -4.0f;
    *(float *)0x700038C4 = 0.0f;
    *(float *)0x700038C8 = 0.0f;
    *(float *)0x700038CC = 1.0f;
    func_001026A0(D_700038D0, D_70003070, D_700038C0);
    func_001028B8(D_700038C0, D_700038A0, D_700038D0);
    *(float *)0x700038CC = 1.0f;
    if (func_0019AFE0(obj, D_700038B0, D_700038C0, 7)) {
        hit |= 1;
    }
    if (func_0019AFE0(obj, D_700038C0, D_700038B0, 7)) {
        hit |= 2;
    }
    if (hit) {
        return hit;
    }

    *(float *)0x700038B0 = 4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 0.0f;
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038A0 = *(float *)0x70003050 + 1.5f * *(float *)0x70003060;
    *(float *)0x700038A8 = *(float *)0x70003058 + 1.5f * *(float *)0x70003068;
    *(float *)0x700038A4 = f - 1.0f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0(D_700038D0, D_70003070, D_700038B0);
    func_001028B8(D_700038B0, D_700038A0, D_700038D0);
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038C0 = -4.0f;
    *(float *)0x700038C4 = 0.0f;
    *(float *)0x700038C8 = 0.0f;
    *(float *)0x700038CC = 1.0f;
    func_001026A0(D_700038D0, D_70003070, D_700038C0);
    func_001028B8(D_700038C0, D_700038A0, D_700038D0);
    *(float *)0x700038CC = 1.0f;
    if (func_0019AFE0(obj, D_700038B0, D_700038C0, 7)) {
        hit |= 1;
    }
    if (func_0019AFE0(obj, D_700038C0, D_700038B0, 7)) {
        hit |= 2;
    }
    if (hit) {
        return hit;
    }

    *(float *)0x700038B0 = 4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 2.0f;
    *(float *)0x700038BC = 1.0f;
    func_001026A0(D_700038C0, D_70003070, D_700038B0);
    func_001028B8(D_700038B0, D_700038A0, D_700038C0);
    *(float *)0x700038BC = 1.0f;
    if (func_0019AFE0(obj, D_700038A0, D_700038B0, 7) & 6) {
        side |= 1;
    }

    *(float *)0x700038B0 = -4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 2.0f;
    *(float *)0x700038BC = 1.0f;
    func_001026A0(D_700038C0, D_70003070, D_700038B0);
    func_001028B8(D_700038B0, D_700038A0, D_700038C0);
    *(float *)0x700038BC = 1.0f;
    if (func_0019AFE0(obj, D_700038A0, D_700038B0, 7) & 6) {
        side |= 2;
    }

    return side ? 0 : -1;
}
