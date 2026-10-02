// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA17 overlay, runtime 0x008251B0 (splat/link name 00825170; overlay code
//  is linked 0x40 below where it runs), 0x2BC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA17; lane OVLC).
// Role: group 0x827F40 member: a part attached to the +0x20 owner. State 0:
//  state 1 when func_001BAD40(self, owner) is 0. State 1: +0x28 counts
//  frames; the +0x24 object's +0x2E bit (1 << +0x2E) gives state 2; else,
//  unless the owner's +4 is 0x270D / 0x270C, by the owner's +0xA: 5
//  func_001C5C90 (and return), 4 func_001C68C0 and the +0x4C method, 3
//  nothing, 0 func_001BA580(self, owner +4) then the default; default
//  func_001C64F0(self, owner +0xC), func_001C68C0, +1 = 1 and the +0x4C
//  method. For frames 521..2282 and 3699..3989 a func_001F4E20 point (0,
//  0xA0, 0, 0x80) at the +0x138 object's matrix times (-1.2367, 0.2497,
//  -0.4078). State 2: state 3, func_001BA540 when the owner's +4 != 0x270D
//  and +0xA == 0. State 3 func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
#define S16(o) (*(short *)(self + (o)))
extern float D_700038A0[];
extern int D_700038B0[];
extern int func_001BAD40(unsigned char *self, unsigned char *own);
extern void func_001BA580(unsigned char *self, int a1);
extern short func_001C64F0(unsigned char *self, float step);
extern void func_001C68C0(unsigned char *self);
extern void func_001C5C90(unsigned char *self);
extern void func_001026A0(void *dst, void *m, void *v);
extern void func_001F4E20(void *pos, void *rgba, float f12);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA17_00825170(unsigned char *self) {
    unsigned char *own = *(unsigned char **)(self + 0x20);
    unsigned char *grp = *(unsigned char **)(self + 0x24);
    unsigned char st = self[4];
    short n;
    switch (st) {
    case 0:
        if (func_001BAD40(self, own) != 0) {
            break;
        }
        self[4] = 1;
    case 1:
        S16(0x28)++;
        if (*(unsigned short *)(grp + 0x2E) & (1 << *(unsigned short *)(self + 0x2E))) {
            self[4] = 2;
        } else {
            if (*(short *)(own + 4) == 0x270D || *(short *)(own + 4) == 0x270C) {
                break;
            }
            switch (*(short *)(own + 0xA)) {
            case 5:
                func_001C5C90(self);
                return;
            case 4:
                func_001C68C0(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            case 3:
                break;
            case 0:
                func_001BA580(self, *(short *)(own + 4));
                func_001C64F0(self, *(float *)(own + 0xC));
                func_001C68C0(self);
                self[1] = 1;
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            default:
                func_001C64F0(self, *(float *)(own + 0xC));
                func_001C68C0(self);
                self[1] = 1;
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            }
        }
        n = S16(0x28);
        if ((n > 520 && n < 2283) || (n > 3698 && n <= 3989)) {
            D_700038A0[0] = -1.2367f;
            D_700038A0[1] = 0.2497f;
            D_700038A0[2] = -0.4078f;
            D_700038A0[3] = 1.0f;
            func_001026A0(D_700038A0, *(unsigned char **)(self + 0x138) + 0x90, D_700038A0);
            D_700038B0[0] = 0;
            D_700038B0[1] = 0xA0;
            D_700038B0[2] = 0;
            D_700038B0[3] = 0x80;
            func_001F4E20(D_700038A0, D_700038B0, 1.0f);
        }
        break;
    case 2:
        self[4] = st + 1;
        if (*(short *)(own + 4) != 0x270D && *(short *)(own + 0xA) == 0) {
            func_001BA540(self);
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}
