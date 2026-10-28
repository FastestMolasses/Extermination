// NEARMISS func_001776E0  (vram 0x001776E0, 0x494 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.53% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 75.61%): the probe-box centre and height are computed into locals before the four box stores (mwcc never moves a load above a store to a literal address), and the tail is `side == 0 ? -1 : 0`. The target has no VU0 instructions, so no asm is used.
// Remaining differences: the obj argument copy and the 4.0f constant are scheduled differently at entry, and the hit tests leave one delay slot unfilled where the original fills it with the next probe address.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Builds two AABB-style probe boxes in scratchpad blocks at 0x700038A0/B0/C0/D0
// around a centre derived from globals 0x70003050/3058 +/- 0.5*0x70003060/3068,
// runs func_0019AFE0 collision probes (mask 7) in two passes (offset +/-1 from
// the float arg f), accumulating a hit bitmask; if any pass hits it returns the
// mask. If neither does, two more probes (mask & 6) set a second bitmask and it
// returns 0 on a side-hit else -1.
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
    float x, y, z;

    x = *(float *)0x70003050 - 0.5f * *(float *)0x70003060;
    z = *(float *)0x70003058 - 0.5f * *(float *)0x70003068;
    y = 1.0f + f;
    *(float *)0x700038B0 = 4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 0.0f;
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038A0 = x;
    *(float *)0x700038A8 = z;
    *(float *)0x700038A4 = y;
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

    x = *(float *)0x70003050 + 1.5f * *(float *)0x70003060;
    z = *(float *)0x70003058 + 1.5f * *(float *)0x70003068;
    y = f - 1.0f;
    *(float *)0x700038B0 = 4.0f;
    *(float *)0x700038B4 = 0.0f;
    *(float *)0x700038B8 = 0.0f;
    *(float *)0x700038BC = 1.0f;
    *(float *)0x700038A0 = x;
    *(float *)0x700038A8 = z;
    *(float *)0x700038A4 = y;
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

    return side == 0 ? -1 : 0;
}
