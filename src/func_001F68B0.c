// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Input-state dispatcher. obj = func_001F6760(); func_001F6850(); then keys on a
// 16-bit code built from two global input bytes (D_00810700<<8 | D_00810701).
// For each recognized key it conditionally re-issues the action on obj via
// func_001F6640 when the matching latch byte (D_0081075D/5E/61/84/85/9E) is
// (or is not) 0xFF. The 0x1301 key fires two distinct objects (&D_0025D2C0,
// &D_0025D270). The default branch routes any other key to func_001F66F0(obj)
// when obj is non-null. func_001F6640/func_001F66F0 take only obj (the a1 seen
// at the call sites is a leftover register).
extern void func_001F6640(int obj);
extern void func_001F66F0(int obj);
extern int func_001F6760(void);
extern void func_001F6850(void);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_0081075D;
extern unsigned char D_0081075E;
extern unsigned char D_00810761;
extern unsigned char D_00810778;
extern unsigned char D_0081077B;
extern unsigned char D_00810784;
extern unsigned char D_00810785;
extern unsigned char D_0081079E;
extern int D_0025D270;
extern int D_0025D2C0;

void func_001F68B0(void) {
    int obj;
    int key;

    obj = func_001F6760();
    func_001F6850();
    key = (D_00810700 << 8) + D_00810701;
    switch (key) {
    case 0x0:
        if (D_0081075D != 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x1:
        if (D_0081075E == 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x2:
        if (D_00810784 == 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x100:
        if (D_0081075E != 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x200:
        if (D_00810761 == 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0xE00:
        if (D_00810784 == 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x1100:
        if (D_00810785 == 0xFF) {
            func_001F6640(obj);
        }
        break;
    case 0x1301:
        if (D_0081079E == 0xFF) {
            func_001F6640((int)&D_0025D2C0);
        }
        if (D_00810778 != 0xFF && D_0081077B != 0xFF) {
            func_001F6640((int)&D_0025D270);
        }
        break;
    default:
        if (obj != 0) {
            func_001F66F0(obj);
        }
        break;
    }
}
