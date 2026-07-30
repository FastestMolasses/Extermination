// NEARMISS func_001BC350  (vram 0x001BC350, 0x204 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// jr-table external-dispatch wall (proven s84) — residuals: the 2-instruction lui/addiu jump-table reloc pair (local @34 vs external jtbl_0026E1C0), plus a scratch-register naming tie in the unlock-bit test (target uses a0/v1/v0 where mwcc233 picks v1/v0/a0 — identical instruction sequence and oper...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS (2026-06-10 s15, verified against the disassembly s85): DOOR
// behavior (placement +0x24 target for class-5 double doors).  `blk` is
// self+0x1F0.  Outer lifecycle byte +0x04:
//   0 INIT  func_001BBDA0 (model bind, +0x34 = door id from +0x2E, scale from
//           link flags 0x40/0x80), then status byte +0x00 = 1
//   1 RUN   inner sub-state byte +0x05 via jtbl_0026E1C0 (below)
//   2/3 FREE func_001AFC10
//   other   nothing
// RUN sub-states:
//   0 closed   model 0x15 (+0x03) and unlock bit (D_00810841[D_00810700] >>
//              self+0x34) set -> func_001BBE40(self, blk, 0) -> sub 3
//              (walk-through); bit clear -> func_001BBE40(self, blk, 1) ->
//              sub 1 (locked seq); other models -> func_001BBE40(self, blk, 0)
//              -> sub 3
//   1 locked   func_001BC0E0 pump; done -> func_001BA1A0(blk, D_0024DBC0)
//              (queue script) -> sub 2
//   2 locked2  func_001BC0E0 pump; done -> +0x0B = 0, sub 0
//   3 opening  func_001BC0E0 pump; done -> sub 4
//   4 commit   func_001BC240: advance clip + func_001BC150 (transition
//              request D_008106B5..B8) -> sub 5
//   5 close    func_001BC290: advance clip; when D_008106B8 == 0 reset clip,
//              +0x0B = 0, sub 0
// RUN then ALWAYS calls func_001BC300 (evaluate articulation via
// func_001C68C0, cull at pos+(0,10,0), run the +0x4C method) - including
// when the sub-state is out of range.  See docs/FINDINGS.md s15.
// func_001BBDA0 is called with no argument set-up in the original ($a0 still
// holds self), so it is declared argument-less here.
// NEARMISS: 99.46% - residuals are the jump-table reloc (local @NN vs. the
// original's external jtbl_0026E1C0) and a scratch-register naming tie in
// the unlock-bit test (v0/v1/a0 permuted; same instruction sequence).

extern void func_001BBDA0();
extern int func_001BBE40(unsigned char *, unsigned char *, int);
extern int func_001BC0E0(unsigned char *, unsigned char *);
extern void func_001BA1A0(unsigned char *, int *);
extern void func_001BC240(unsigned char *, unsigned char *);
extern int func_001BC290(unsigned char *, unsigned char *);
extern void func_001BC300(unsigned char *);
extern void func_001AFC10(unsigned char *);

extern unsigned char D_00810700;
extern unsigned char D_00810841[];
extern int D_0024DBC0[];

void func_001BC350(unsigned char *self) {
    unsigned char *blk;

    blk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0();
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[3] == 0x15) {
                if (D_00810841[D_00810700] & (1 << *(short *)(self + 0x34))) {
                    if (func_001BBE40(self, blk, 0) != 0) {
                        self[5] = 3;
                    }
                } else {
                    if (func_001BBE40(self, blk, 1) != 0) {
                        self[5] = self[5] + 1;
                    }
                }
            } else {
                if (func_001BBE40(self, blk, 0) != 0) {
                    self[5] = 3;
                }
            }
            break;
        case 1:
            if (func_001BC0E0(self, blk) != 0) {
                func_001BA1A0(blk, D_0024DBC0);
                self[5] = self[5] + 1;
            }
            break;
        case 2:
            if (func_001BC0E0(self, blk) != 0) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        case 3:
            if (func_001BC0E0(self, blk) != 0) {
                self[5] = self[5] + 1;
            }
            break;
        case 4:
            func_001BC240(self, blk);
            self[5] = self[5] + 1;
            break;
        case 5:
            if (func_001BC290(self, blk) != 0) {
                self[5] = 0;
            }
            break;
        }
        func_001BC300(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
