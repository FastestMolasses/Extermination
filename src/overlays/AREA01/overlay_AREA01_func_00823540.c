// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code is
// linked 0x40 below where it runs), 0x24C bytes. Byte-identical C, linked from
// its compiled object: the text and the jump table (7 entries at runtime
// 0x0082CB80) match the original. The compiled .rodata is placed by
// tools/overlay/jt_pin.py at the table's original address, with the table
// address and its case-label entries resolved at runtime addresses (link +
// 0x40), which is what the original stores.
// Role: shaft door owner (placement [12]). +4: 0 calls func_001BBDA0 and sets
//  +0 = 1; 1 runs the seven-step program selected by +5 through a jump table,
//  then sets byte +0xB of the record at +0x1C to 1 when D_008107D9 is 0x81
//  (else 0) and calls func_001BC300; 2 and 3 call func_001AFC10.
extern unsigned char D_008107D9;
extern char D_overlay_AREA01_00829860[];
extern char D_overlay_AREA01_008298E0[];
extern void func_001BBDA0(unsigned char *self);
extern int func_001BBE40(unsigned char *self, unsigned char *talk, int mode);
extern int func_001BC0E0(unsigned char *self, unsigned char *talk);
extern void func_001BC240(unsigned char *self, unsigned char *talk);
extern int func_001BC290(unsigned char *self, unsigned char *talk);
extern void func_001BC300(unsigned char *self);
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void overlay_AREA01_func_00823540(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    unsigned char *node = *(unsigned char **)(self + 0x1C);
    int gate;

    switch (self[4]) {
    case 0:
        func_001BBDA0(self);
        self[0] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (D_008107D9 == 0x81) {
                if (func_001BBE40(self, talk, 0)) {
                    self[5] = 3;
                }
            } else if (func_001BBE40(self, talk, 1)) {
                self[5]++;
            }
            break;
        case 1:
            if (func_001BC0E0(self, talk)) {
                func_001BA1A0(talk, D_overlay_AREA01_00829860);
                self[5]++;
            }
            break;
        case 2:
            if (func_001BC0E0(self, talk)) {
                gate = D_008107D9;
                if (gate == 0 || gate == 0x80) {
                    D_008107D9 = 0x80;
                    func_001BA1A0(talk, D_overlay_AREA01_008298E0);
                    func_001BA1F0(self);
                    self[5] = 6;
                } else {
                    self[0xB] = 0;
                    self[5] = 0;
                }
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
        case 6:
            if (func_001BA1F0(self)) {
                self[0xB] = 0;
                self[5] = 0;
            }
            break;
        }
        if (D_008107D9 == 0x81) {
            node[0xB] = 1;
        } else {
            node[0xB] = 0;
        }
        func_001BC300(self);
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
