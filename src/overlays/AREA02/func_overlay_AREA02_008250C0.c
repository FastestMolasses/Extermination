// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00825100 (splat/link name 008250C0; overlay
// code is linked 0x40 below where it runs), 0x420 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Role: state +4. 0: func_001B0FD0, +0x30 = 0x829190, +0 = 1, +0x38 = 0,
//  +4 = 1, D_0081083F = 0. 1 with +3 == 7: +5 0 waits for D_0081083F (+5 =
//  1, script 0x828E10); +5 1 by D_0081083F: 2 calls 0x825520 and continues
//  as 1: +0x38 is 0.00374 when the value is 2 and zero otherwise; at script
//  end D_0081083F, +5 and +0x38 are cleared; func_001C6380 and func_001A2370(self, +0xD0). Then
//  func_001C6380 and two func_001B18F0 tests with scratchpad pairs
//  (58,0,0,1)/(0,0,58,1) and (40,0,40,1)/(-40,0,40,1); the +0x4C callback
//  runs when either returns nonzero. 1 with +3 == 4: +5 0 waits for +0xB
//  bit 2 (func_001B6F00(self, (0,0,5,1), pi), script 0x828C50, +5 = 1);
//  1 sets D_0081083F = 1, +5 = 2 at script end; 2 clears D_0081083F, +0xB
//  and +5 once D_0081083F is 0. If D_00810761 == 0, func_001F4BF0 at
//  +0xB0 + (1, 15, 0) with (0, 0x80, 0, 0x80). Then func_001C6380,
//  func_001B17A0 and the callback. 2, 3: func_001AFC10.
// Matching: 0x700038B0 is one extern union (float and word views).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081083F;
extern unsigned char D_00810761;
extern float D_700038A0[4];
typedef union {
    float f[4];
    int i[4];
} Quad;
extern Quad D_700038B0;
extern char D_overlay_AREA02_00829190[];
extern char D_overlay_AREA02_00828E10[];
extern char D_overlay_AREA02_00828C50[];
extern int func_001B0FD0(unsigned char *self);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *a1);
extern int func_001B18F0(unsigned char *self, float *a, float *b);
extern void func_001B6F00(unsigned char *owner, const float *local_point, float yaw_offset);
extern void func_001F4BF0(void *a, void *b);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA02_00825520(void);

void func_overlay_AREA02_008250C0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001B0FD0(self);
        *(char **)(self + 0x30) = D_overlay_AREA02_00829190;
        self[0] = 1;
        *(int *)(self + 0x38) = 0;
        self[4] = 1;
        D_0081083F = 0;
        break;
    case 1:
        if (self[3] == 7) {
            switch (self[5]) {
            case 0:
                if (D_0081083F != 0) {
                    self[5] = 1;
                    func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00828E10);
                }
                break;
            case 1:
                switch (D_0081083F) {
                case 0:
                    break;
                case 2:
                    func_overlay_AREA02_00825520();
                case 1:
                    if (D_0081083F == 2) {
                        *(float *)(self + 0x38) = 0.00373999146f;
                    } else {
                        *(float *)(self + 0x38) = 0.0f;
                    }
                    if (func_001BA1F0(self)) {
                        D_0081083F = 0;
                        self[5] = 0;
                        *(float *)(self + 0x38) = 0.0f;
                    }
                    func_001C6380(self);
                    func_001A2370(self, self + 0xD0);
                    break;
                }
                break;
            }
            func_001C6380(self);
            D_700038A0[0] = 58.0f;
            D_700038A0[1] = 0.0f;
            D_700038A0[2] = 0.0f;
            D_700038A0[3] = 1.0f;
            D_700038B0.f[0] = 0.0f;
            D_700038B0.f[1] = 0.0f;
            D_700038B0.f[2] = 58.0f;
            D_700038B0.f[3] = 1.0f;
            if (func_001B18F0(self, D_700038A0, D_700038B0.f) == 0) {
                D_700038A0[0] = 40.0f;
                D_700038A0[1] = 0.0f;
                D_700038A0[2] = 40.0f;
                D_700038A0[3] = 1.0f;
                D_700038B0.f[0] = -40.0f;
                D_700038B0.f[1] = 0.0f;
                D_700038B0.f[2] = 40.0f;
                D_700038B0.f[3] = 1.0f;
                if (func_001B18F0(self, D_700038A0, D_700038B0.f) == 0) {
                    break;
                }
            }
            (*(ActorFn *)(self + 0x4C))(self);
        } else if (self[3] == 4) {
            switch (self[5]) {
            case 0:
                if (self[0xB] & 4) {
                    D_700038A0[0] = 0.0f;
                    D_700038A0[1] = 0.0f;
                    D_700038A0[2] = 5.0f;
                    D_700038A0[3] = 1.0f;
                    func_001B6F00(self, D_700038A0, 3.1415927f);
                    func_001BA1A0(talk, (unsigned char *)D_overlay_AREA02_00828C50);
                    self[5] = 1;
                }
                break;
            case 1:
                if (func_001BA1F0(self)) {
                    D_0081083F = 1;
                    self[5] = 2;
                }
                break;
            case 2:
                switch (D_0081083F) {
                case 0:
                    D_0081083F = 0;
                    self[0xB] = 0;
                    self[5] = 0;
                    break;
                case 1:
                    break;
                }
                break;
            }
            if (D_00810761 == 0) {
                D_700038A0[0] = *(float *)(self + 0xB0) + 1.0f;
                D_700038A0[1] = *(float *)(self + 0xB4) + 15.0f;
                D_700038A0[2] = *(float *)(self + 0xB8);
                D_700038A0[3] = 1.0f;
                D_700038B0.i[0] = 0;
                D_700038B0.i[1] = 0x80;
                D_700038B0.i[2] = 0;
                D_700038B0.i[3] = 0x80;
                func_001F4BF0(D_700038A0, D_700038B0.f);
            }
            func_001C6380(self);
            func_001B17A0(self);
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
