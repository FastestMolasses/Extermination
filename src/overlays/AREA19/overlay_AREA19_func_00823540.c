// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x1EC bytes.
// Byte-identical once linked (lane A13C): overlay_match.py reports 99.98
//  because the switch table is .rodata that needs pinning;
//  tools/overlay/jt_pin.py places the compiled table at the original address
//  0x82F800 at link.
// Role: door [27] (sub 0 placement, door id 2: room move to entry 1 / 2). A
//  six-way switch on +5 compiled to a jump table (the original's table is at
//  0x82F800): 0 steps to 3 when func_001BBE40(self, talk, 0) is set while
//  D_00810841[D_00810700] has bit (+0x34) set, otherwise to 1 when
//  func_001BBE40(self, talk, 1) is set; 1 starts script 0x82AD50 on
//  func_001BC0E0; 2 on func_001BC0E0 calls func_001C4760(0xA, 1) unless
//  D_00810CCD is set and clears +0xB / +5; 3 steps on func_001BC0E0; 4
//  func_001BC240 and steps; 5 returns to 0 on func_001BC290. Then
//  func_001BC300. State 0 func_001BBDA0(self), +0 = 1; states 2/3
//  func_001AFC10.
extern unsigned char D_00810841[];
extern char D_overlay_AREA19_0082AD50[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001BA1A0(unsigned char *talk, void *script);
extern void func_001C4760(int id, int on);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA19_func_00823540(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_00810841[*(unsigned char *)0x810700] & (1 << *(short *)(self + 0x34))) {
                if (func_001BBE40(self, talk, 0)) {
                    self[5] = 3;
                }
            } else if (func_001BBE40(self, talk, 1)) {
                self[5]++;
            }
            break;
        case 1:
            if (func_001BC0E0(self, talk)) {
                func_001BA1A0(talk, D_overlay_AREA19_0082AD50);
                self[5]++;
            }
            break;
        case 2:
            if (func_001BC0E0(self, talk)) {
                if (*(unsigned char *)0x810CCD == 0) {
                    func_001C4760(0xA, 1);
                }
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        case 3:
            if (func_001BC0E0(self, talk)) {
                self[5]++;
            }
            break;
        case 4:
            func_001BC240(self, talk);
            self[5]++;
            break;
        case 5:
            if (func_001BC290(self, talk)) {
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
