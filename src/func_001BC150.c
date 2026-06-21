// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS (FINDINGS "AREA TRANSITION LIFECYCLE"): door transition COMMIT,
// called from sub-state 4 (func_001BC240). Record = D_0024E140[D_00810700] +
// 4*(door id +0x34 & 0x7F). Door-id bit 7 set -> inter-AREA change:
// func_001B0C00(4) (fade + audio fades), request {B8=1, B5=rec[0] next area,
// B7=rec[1] entry, B6=rec[2]?rec[3]:0xFF sub}. Bit 7 clear -> same-area room
// move: func_001AEDE0(4,0) fade only, {B8=2, B7=rec[side latch +0x2E]}.
// KEY MATCH NOTE: compute rec = D_0024E140[D_00810700] FIRST (before reading the
// door fields) so the address-pair hoist and temp-register pool match CW's order.
extern unsigned char *D_0024E140[];
extern unsigned char D_00810700;
extern unsigned char D_008106B5, D_008106B6, D_008106B7, D_008106B8;
extern void func_001B0C00(int speed);
extern void func_001AEDE0(int speed, int mode);

void func_001BC150(unsigned char *self) {
    unsigned char *rec;
    unsigned char id;
    short hw;

    rec = D_0024E140[D_00810700];
    id = self[0x34];
    hw = *(short *)(self + 0x34);
    rec = rec + ((id & 0x7F) * 4);
    if (hw & 0x80) {
        func_001B0C00(4);
        D_008106B8 = 1;
        D_008106B5 = rec[0];
        D_008106B7 = rec[1];
        if (rec[2] == 0) {
            D_008106B6 = 0xFF;
        } else {
            D_008106B6 = rec[3];
        }
    } else {
        func_001AEDE0(4, 0);
        D_008106B8 = 2;
        D_008106B7 = rec[*(unsigned short *)(self + 0x2E)];
    }
}
