// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Selects an animation/clip index into obj[0x7F] based on the global mode byte
// D_00810700 and a global float D_00810368, then drives the chosen clip.
//   mode 0xD: if D_00810368 >= 970.0 -> index 1; else if a bounds test against
//             D_00245A60 passes -> index 2; else if against D_00245AA0 -> 3.
//   other modes: if D_00810368 >= 1041.04 -> index 4.
// If no index was set (still 0) it just copies obj into obj+0x10 and returns.
// Otherwise it clears the timer at obj[0x64], copies the clip record
// (D_00245AE0 + (index-1)*0x10) into obj+0x10, stages the scratch blocks at
// 0x700038A0/0x700038B0, and on a positive func_001B13F0 result (threshold 20.0)
// sets the high (sign) bit of obj[0x7F].
//
// Matched with mwcc 2.3.3; pinned 991202 only reaches 84.86%. objdiff 100.0.
extern int func_001B1EA0(int, void *, void *, int);
extern int func_001B13F0(void *a, void *b, float s);
extern void func_00102948(void *dst, void *src);
extern unsigned char D_00810700;
extern float D_00810368;
extern char D_00810360[];
extern char D_00245A60[];
extern char D_00245AA0[];
extern char D_00245AE0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001471E0(char *self, char *obj) {
    int idx;

    obj[0x7F] = 0;
    if (D_00810700 == 0xD) {
        if (D_00810368 >= 970.0f) {
            obj[0x7F] = 1;
        } else if (func_001B1EA0(0, D_00810360, D_00245A60, 4) != 0) {
            obj[0x7F] = 2;
        } else if (func_001B1EA0(0, D_00810360, D_00245AA0, 4) != 0) {
            obj[0x7F] = 3;
        }
    } else if (D_00810368 >= 1041.0400390625f) {
        obj[0x7F] = 4;
    }
    idx = obj[0x7F];
    if (idx == 0) {
        func_00102948(obj + 0x10, obj);
        return;
    }
    *(short *)(obj + 0x64) = 0;
    func_00102948(obj + 0x10, D_00245AE0 + ((obj[0x7F] - 1) << 4));
    func_00102948(D_700038A0, obj + 0x10);
    func_00102948(D_700038B0, self + 0xB0);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    if (func_001B13F0(D_700038A0, D_700038B0, 20.0f) != 0) {
        obj[0x7F] = obj[0x7F] | 0xFFFFFF80;
    }
}
