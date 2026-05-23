extern unsigned char D_00810118[];
extern unsigned char D_00810119[];
extern unsigned char D_0081011C[];
extern short D_00810120[];
extern short D_00810122[];
extern volatile unsigned char D_00810708[];
extern volatile unsigned char D_00810709[];
extern volatile unsigned char D_0081070C[];
extern volatile short D_00810754[];
extern volatile short D_00810756[];

extern void func_001AF470(unsigned char a0);

void func_001AF1C0(void) {
    unsigned char a2 = D_00810118[0];
    unsigned char a1 = D_00810119[0];
    unsigned char a0 = D_0081011C[0];
    short v1 = D_00810120[0];
    short v0 = D_00810122[0];
    D_00810708[0] = a2;
    D_00810709[0] = a1;
    D_0081070C[0] = a0;
    D_00810754[0] = v1;
    D_00810756[0] = v0;
    func_001AF470(D_00810708[0]);
}
