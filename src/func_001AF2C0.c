// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 80.84% NEARMISS).
// Fixes read from the original instructions:
// - The trailing global-init stores (D_00810703.. through D_00810CAE) are
//   declared volatile: the original keeps every store in source order and
//   reloads nothing across them, which mwcc only does for volatile objects.
//   Non-volatile externs let the scheduler group the stores by constant value
//   (the old "scheduler wall"). Neighbouring files already declare globals in
//   this 0x00810700 block volatile.
//
// Resets the 0x640-byte state block at D_00810700: zero-fills it, copies five
// fields from D_00810118.. into D_00810708.., calls func_001AF470 with the
// first copied byte, then writes the default values of the remaining fields
// (0xFF markers, 100.0f, flags set to 1, counters 5/7, and 30/60 after the
// func_001C40B0(0x10, 2) call).
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
extern volatile unsigned char D_00810703;
extern volatile unsigned char D_00810704;
extern volatile unsigned char D_00810705;
extern volatile unsigned char D_00810706;
extern volatile unsigned char D_00810707;
extern volatile float D_00810858;
extern volatile int D_0081085C;
extern volatile unsigned char D_00810C60;
extern volatile unsigned char D_00810D20;
extern volatile unsigned char D_00810D21;
extern volatile unsigned char D_00810D22;
extern volatile unsigned char D_00810D23;
extern volatile unsigned char D_00810C7B;
extern volatile unsigned char D_00810CB7;
extern volatile short D_00810CB2;
extern volatile unsigned char D_00810C69;
extern volatile unsigned char D_00810C64;
extern volatile unsigned char D_00810C6B;
extern volatile unsigned char D_00810CA4;
extern volatile unsigned char D_00810CA6;
extern volatile unsigned char D_00810CA5;
extern volatile unsigned char D_00810CA7;
extern volatile unsigned char D_00810C61;
extern volatile unsigned char D_00810C62;
extern volatile short D_00810CB4;
extern volatile unsigned char D_00810D3C;
extern volatile short D_00810CAA;
extern volatile short D_00810CA8;
extern volatile short D_00810CB0;
extern volatile short D_00810CAC;
extern volatile short D_00810CAE;

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
