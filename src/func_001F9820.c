// NEARMISS func_001F9820  (vram 0x001F9820, 0x300 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.35% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling permutation in channels 3 and 4: mwcc places the D_00275B24/B20 source load and the split lui/ori of the 0x467FFC00 constant a couple of slots away from the boot order (independent global stores reordered). Plus cosmetic data-symbol granularity on the flag bytes (D_00282154...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// objdiff 95.4% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4);
// the pinned 991202 build reaches 75.4%. The LOGIC and STRUCTURE are fully
// recovered; the residual is a genuine compiler artifact (see RESIDUAL below).
//
// Four-channel audio-voice init. Calls func_0011A4B8() once, clears three flag
// bytes at D_00282154[0..2] and the gp word D_00275B2C, then allocates four
// voices via func_0011A2B0(0) -> v0..v3. For each voice it fills a 6-word
// parameter block on the stack and submits it with func_0011A4E8(blk), then
// programs the channel: func_0011A608((1<<voice), capL, capR) and
// func_0011A658((1<<voice), 0xBB80). The per-channel state (voice index, channel
// bitmask sign-extended to 64-bit, the D_00275Bxx source pointer, 0x10000, and
// 0x467FFC00) is written into a strided global table starting at D_00281FD4
// (stride 0x60). Note the asymmetry faithfully reproduced from the boot: channel
// 1 stores the COMBINED mask (1<<v0)|(1<<v1) at D_00281FD8, and channel 2 only
// records its voice index at D_002820F4 (its mask is folded into channel 1's).
// A608 caps differ per channel: ch1 (0x3FFF,0), ch2 (0,0x3FFF), ch3/ch4
// (0x3FFF,0x3FFF). Finishes with func_001FA570(D_00275B20).
//
// RESIDUAL (NOT the clean-store nop): in channels 3 and 4 mwcc schedules the
// D_00275B24/B20 source load and the split materialization of the 0x467FFC00
// constant a couple of slots away from the boot's order (independent global
// stores reordered) -> instruction-scheduling permutation. The flag bytes
// D_00282154[1]/[2] also render as a data-symbol-granularity delta (boot has
// separate byte symbols D_00282155/56; same final addresses) -> cosmetic.
extern int D_00275B20;
extern int D_00275B24;
extern int D_00275B28;
extern int D_00275B2C;
extern int D_00281FD4[2];
extern long long D_00281FD8;
extern int D_00281FE4[2];
extern int D_00281FE8[2];
extern int D_00282028[2];
extern int D_00282034[2];
extern long long D_00282038;
extern int D_00282044[2];
extern int D_00282048[2];
extern int D_00282088[2];
extern int D_00282094[2];
extern long long D_00282098;
extern int D_002820A4[2];
extern int D_002820A8[2];
extern int D_002820E8[2];
extern int D_002820F4[2];
extern char D_00282154[5];

extern void func_0011A4B8(void);
extern int func_0011A2B0(int);
extern void func_0011A4E8(int *blk);
extern void func_0011A608(unsigned long long mask, int a1, int a2);
extern void func_0011A658(unsigned long long mask, int a1);
extern void func_001FA570(int a);

void func_001F9820(void) {
    int v0, v1, v2, v3;
    int blk[6];

    func_0011A4B8();
    D_00282154[0] = 0;
    D_00282154[1] = 0;
    D_00275B2C = 0;
    D_00282154[2] = 0;
    v0 = func_0011A2B0(0);
    v1 = func_0011A2B0(0);
    v2 = func_0011A2B0(0);
    v3 = func_0011A2B0(0);

    blk[0] = v0;
    blk[1] = 0x20002;
    blk[2] = D_00275B28 + 0x400;
    blk[3] = 0x10000;
    blk[4] = 0x5010;
    blk[5] = 0x4000;
    func_0011A4E8(blk);
    func_0011A608((unsigned long long)1 << v0, 0x3FFF, 0);
    func_0011A658((unsigned long long)1 << v0, 0xBB80);
    D_00281FD4[0] = v0;
    D_00281FD8 = (int)((1 << v0) | (1 << v1));
    D_00281FE4[0] = D_00275B28;
    D_00281FE8[0] = 0x10000;
    D_00282028[0] = 0x467FFC00;

    blk[0] = v1;
    blk[1] = 0x20002;
    blk[2] = D_00275B28;
    blk[3] = 0x10000;
    blk[4] = 0x9010;
    blk[5] = 0x4000;
    D_002820F4[0] = v1;
    func_0011A4E8(blk);
    func_0011A608((unsigned long long)1 << v1, 0, 0x3FFF);
    func_0011A658((unsigned long long)1 << v1, 0xBB80);

    blk[0] = v2;
    blk[1] = 0x10000;
    blk[2] = D_00275B24;
    blk[3] = 0x10000;
    blk[4] = 0xD010;
    blk[5] = 0x4000;
    func_0011A4E8(blk);
    func_0011A608((unsigned long long)1 << v2, 0x3FFF, 0x3FFF);
    func_0011A658((unsigned long long)1 << v2, 0xBB80);
    D_00282034[0] = v2;
    D_00282038 = (int)(1 << v2);
    D_00282044[0] = D_00275B24;
    D_00282048[0] = 0x10000;
    D_00282088[0] = 0x467FFC00;

    blk[0] = v3;
    blk[1] = 0x10000;
    blk[2] = D_00275B20;
    blk[3] = 0x10000;
    blk[4] = 0x11010;
    blk[5] = 0x4000;
    func_0011A4E8(blk);
    func_0011A608((unsigned long long)1 << v3, 0x3FFF, 0x3FFF);
    func_0011A658((unsigned long long)1 << v3, 0xBB80);
    D_00282094[0] = v3;
    D_00282098 = (int)(1 << v3);
    D_002820A4[0] = D_00275B20;
    D_002820A8[0] = 0x10000;
    D_002820E8[0] = 0x467FFC00;

    func_001FA570(D_00275B20);
}
