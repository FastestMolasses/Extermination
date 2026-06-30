// NEARMISS func_001AF2C0  (vram 0x001AF2C0, 0x1A4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling permutation of trailing independent global stores. Insns 0-29 (zero-fill, 5-field copy from D_00810118.. to D_00810708.., func_001AF470 call) byte-identical; all store values/targets and the func_001C40B0(0x10,2) call correct. mwcc list scheduler batches same-value stores b...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: best 80.9% (mwcc233), 77.9% (mwcc991). Body/logic fully recovered:
// the func_00121A28 zero-fill, the 5-field copy from D_00810118.. to D_00810708..,
// the func_001AF470(D_00810708) call, all global-init store VALUES/TARGETS, and the
// func_001C40B0(0x10,2) call are byte-exact through the call sequence (insns 0-29
// identical). WALL: instruction-scheduling permutation of the trailing independent
// global stores. mwcc's list scheduler batches the same-value stores by constant
// register affinity (groups the 0x00/0x01/0xff stores and hoists the 0x42c80000
// float-100 setup), whereas CW 2.3.1 emitted them in source order. Not the
// clean-store delay-slot-nop case, so mwcc 2.3.3 does not close it; reordering source
// statements and shared-temp liveness hints (tested) do not change the schedule.
// Pure scheduler artifact -> orchestrator permuter territory.
extern char D_00810700[];
extern unsigned char D_00810118;
extern unsigned char D_00810119;
extern unsigned char D_0081011C;
extern short D_00810120;
extern short D_00810122;
extern unsigned char D_00810708;
extern unsigned char D_00810709;
extern unsigned char D_0081070C;
extern short D_00810754;
extern short D_00810756;
extern unsigned char D_00810703;
extern unsigned char D_00810704;
extern unsigned char D_00810705;
extern unsigned char D_00810706;
extern unsigned char D_00810707;
extern float D_00810858;
extern int D_0081085C;
extern unsigned char D_00810C60;
extern unsigned char D_00810D20;
extern unsigned char D_00810D21;
extern unsigned char D_00810D22;
extern unsigned char D_00810D23;
extern unsigned char D_00810C7B;
extern unsigned char D_00810CB7;
extern short D_00810CB2;
extern unsigned char D_00810C69;
extern unsigned char D_00810C64;
extern unsigned char D_00810C6B;
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA6;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA7;
extern unsigned char D_00810C61;
extern unsigned char D_00810C62;
extern short D_00810CB4;
extern unsigned char D_00810D3C;
extern short D_00810CAA;
extern short D_00810CA8;
extern short D_00810CB0;
extern short D_00810CAC;
extern short D_00810CAE;

extern void func_00121A28(char *, int, int);
extern void func_001AF470();
extern void func_001C40B0(int, int);

void func_001AF2C0(void) {
    func_00121A28(D_00810700, 0, 0x640);
    D_00810708 = D_00810118;
    D_00810709 = D_00810119;
    D_0081070C = D_0081011C;
    D_00810754 = D_00810120;
    D_00810756 = D_00810122;
    func_001AF470(D_00810708);
    D_00810703 = 0xFF;
    D_00810704 = 0xFF;
    D_00810705 = 0xFF;
    D_00810706 = 0;
    D_00810707 = 0;
    D_00810858 = 100.0f;
    D_0081085C = 0;
    D_00810C60 = 0;
    D_00810D20 = 1;
    D_00810D21 = 1;
    D_00810D22 = 1;
    D_00810D23 = 1;
    D_00810C7B = 1;
    D_00810CB7 = 0;
    D_00810CB2 = 0;
    D_00810C69 = 1;
    D_00810C64 = 1;
    D_00810C6B = 1;
    D_00810CA4 = 0xFF;
    D_00810CA6 = 0;
    D_00810CA5 = 5;
    D_00810CA7 = 7;
    D_00810C61 = 0;
    func_001C40B0(0x10, 2);
    D_00810C62 = 0x1E;
    D_00810CB4 = 0x3C;
    D_00810D3C = 0;
    D_00810CAA = 0;
    D_00810CA8 = 0;
    D_00810CB0 = 0;
    D_00810CAC = 0;
    D_00810CAE = 0;
}
