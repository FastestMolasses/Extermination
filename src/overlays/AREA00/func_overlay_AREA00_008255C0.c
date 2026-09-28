// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA00 overlay, runtime 0x00825600 (splat/link name 008255C0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: behaviour by kind byte +0xD. State 0: kind 2 copies +0xB0 to +0xA0
//  and, when D_008107DC bit 1 is set, moves to (155, -48, -1590); kind 0x15
//  offsets from the record at +0x18. State 1: kind 2 starts script 0x82A0E0
//  (bit 1 set) or 0x829BE0 once D_008107DC bit 0 is set, then emits four
//  points (func_001F5940(3, ...)) from 0x82D440; kind 7 writes 85 + (y - 25)
//  of the record two links up into the object at D_00275B40 + 4 (+0x80);
//  other kinds follow the record at +0x18.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_008107DC[];
extern char *D_00275B40;
extern char D_overlay_AREA00_0082A0E0[];
extern char D_overlay_AREA00_00829BE0[];
extern char D_overlay_AREA00_0082D440[];
extern int func_001B0FD0(unsigned char *self);
extern void func_00102948(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001C6380(unsigned char *self);
extern void func_001A2370(unsigned char *self, void *a1);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001F5940(int a0, float *a1, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_008255C0(unsigned char *self) {
    float v[4];
    unsigned char *talk = self + 0x1F0;
    unsigned char *o;
    int flags;
    float d;
    int i;
    char *p;
    switch (self[4]) {
    case 0:
        if (func_001B0FD0(self) == 0) {
            if (self[0xD] == 2) {
                func_00102948(self + 0xA0, self + 0xB0);
                if (D_008107DC[0] & 2) {
                    *(float *)(self + 0xB0) = 155.0f;
                    *(float *)(self + 0xB4) = -48.0f;
                    *(float *)(self + 0xB8) = -1590.0f;
                    *(float *)(self + 0xBC) = 1.0f;
                }
            } else if (self[0xD] == 0x15) {
                func_001028D0(self + 0xA0, self + 0xB0, *(unsigned char **)(self + 0x18) + 0xA0);
            }
            func_001C6380(self);
            func_001A2370(self, self + 0xD0);
        }
        break;
    case 1:
        if (self[0xD] == 2) {
            switch (self[5]) {
            case 0:
                flags = D_008107DC[0];
                if (flags & 1) {
                    if (flags & 2) {
                        self[5]++;
                        func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_0082A0E0);
                    } else {
                        self[5]++;
                        func_001BA1A0(talk, (unsigned char *)D_overlay_AREA00_00829BE0);
                    }
                }
                break;
            case 1:
                if (func_001BA1F0(self) != 0) {
                    self[5] = 0;
                }
                func_001A2370(self, self + 0xD0);
                break;
            }
            func_001C6380(self);
            self[1] = 1;
            func_001B1B70(self);
            (*(ActorFn *)(self + 0x4C))(self);
            p = D_overlay_AREA00_0082D440;
            for (i = 0; i < 4; i++) {
                func_001026A0(v, self + 0xD0, p);
                func_001F5940(3, v, 0);
                p += 0x10;
            }
        } else if (self[0xD] == 7) {
            o = *(unsigned char **)(*(unsigned char **)(self + 0x18) + 0x18);
            d = *(float *)(o + 0xB4) - 25.0f;
            if (85.0f != d) {
                *(float *)(*(char **)(D_00275B40 + 4) + 0x80) = 85.0f + d;
            }
            func_001C6380(self);
            self[1] = 1;
            func_001B1B70(self);
            func_001A2370(self, self + 0xD0);
            (*(ActorFn *)(self + 0x4C))(self);
        } else {
            o = *(unsigned char **)(self + 0x18);
            func_001028B8(self + 0xB0, o + 0xB0, self + 0xA0);
            *(float *)(self + 0xBC) = 1.0f;
            func_001C6380(self);
            if (o[1] != 0) {
                func_001A2370(self, self + 0xD0);
                self[1] = 1;
                func_001B1B70(self);
            }
            (*(ActorFn *)(self + 0x4C))(self);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}
