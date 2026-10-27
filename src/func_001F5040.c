// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 80.45% NEARMISS).
// Fixes read from the original instructions:
// - Wrong body: the frame-0..3 clip events pass the constant 2 as their last
//   argument, not the frame counter.
// - The parameter is an int handle copied into a char pointer local; that
//   copy (not declaration order) puts the object in the lowest saved register
//   and the 0x1F0 sub-block pointer in the next one.
// - The 128-bit constant is a 4-float struct copy (a TI-mode integer folds
//   the address into the load).
// - Every switch lists its cases in the reverse of the original compare
//   order (mwcc emits compares last-label-first); the sub-type event switch
//   has its default first, the per-frame switch has case 3, cases 0-2,
//   default, then case 15, and the base-pose seed and the frame-3 check are
//   switches with a default arm.
// - The ease toward -128 is a compound assignment on the field; the
//   integrate/decay tail uses += and *=.
//
// Character/actor animation-controller init + per-frame tick, dispatched on
// the state byte at arg0+4 (0=first-time init, 1=running, 2/3=already
// torn-down/despawned).
//
// state 2 or 3: forward straight to func_001AFC10(arg0) (actor free/pool
//   return) and stop.
// state 0 (first-time init): post an anim-clip event keyed off the sub-type
//   byte at arg0+0xD (via func_001C6120(D_0028A56C, tableIndex) ->
//   func_001CA5E0(arg0, id, 2)); then read the bone count into arg0+0xC
//   (func_001C6150(arg0+0x44)); if the level's max-actor threshold
//   D_00275BCC is below that count, mark state=3 and bail (over budget).
//   Else allocate one bone slot per count into the arg0+0x110 array
//   (func_001AF780), stash the count at arg0+9, run
//   anim_bone_array_setup/bone_init_default_1, seed a small random jitter
//   (0.15 +/- 0.05*rand) into arg0+0x60/0x64/0x68 and the mirrored
//   arg0+0x1F0+0x40/0x44/0x48 step fields, seed the base pose
//   arg0+0x80/0x84/0x88 (-96 for sub-type 3/4, else 0), zero arg0+0x8C,
//   copy the 128-bit constant D_0025AD70 through func_001D80E0(arg0+0x100,
//   &tmp), reset the frame counter arg0+0x28, and fall into state 1's
//   per-frame body with state bumped to 1.
// state 1 (per-frame): bump the frame counter arg0+0x28 (post-increment);
//   at 0xF it resets to state 2 (done); at 3 (and 2/1/0) it posts more
//   clip-transition events keyed by sub-type/frame via func_001C6120/
//   func_001CA5E0, or (frame 3, sub-type not in {1,2,3,4}) kicks
//   func_001F4F90(arg0, 2.4f); the default frame drives an exponential
//   ease of arg0+0x80/84/88 toward -128 (t=0.35). Every frame then
//   integrates arg0+0x60/64/68 by the step fields at +0x1F0+0x40/44/48,
//   decays those step fields by 0.8, and finishes with func_001C63D0(arg0),
//   func_001B17A0(arg0), and the object's own vtable tick at arg0+0x4C.
//
// Matched with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// scores 75%.
struct vec { float x, y, z, w; };
extern struct vec D_0025AD70;
extern short D_00275BCC;
extern int D_0028A56C;
extern void anim_bone_array_setup(int a0);
extern void bone_init_default_1(char *arg0);
extern int func_00122BB8(void);
extern int func_001AF780(void);
extern void func_001AFC10(char *handle);
extern void func_001B17A0(char *self);
extern char *func_001C6120(char *a0, int a1);
extern unsigned char func_001C6150(void *a0);
extern void func_001C63D0(char *obj);
extern void func_001CA5E0(int *obj, int field, int arg);
extern void func_001D80E0(int a0, int a1);
extern void func_001F4F90(char *arg0, float f13);

void func_001F5040(int self) {
    char *arg0 = (char *)self;
    struct vec tmp = D_0025AD70;
    char *step;
    unsigned char state;
    unsigned char sub;

    step = arg0 + 0x1F0;
    state = *(unsigned char *)(arg0 + 4);
    switch (state) {
    case 0:
        sub = *(unsigned char *)(arg0 + 0xD);
        switch (sub) {
        default:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xD), 2);
            break;
        case 3:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xD), 2);
            break;
        case 1:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xB), 2);
            break;
        case 4:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xE), 2);
            break;
        case 2:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xF), 2);
            break;
        }
        *(unsigned char *)(arg0 + 0xC) = func_001C6150((void *)*(int *)(arg0 + 0x44));
        if (D_00275BCC < (int)*(unsigned char *)(arg0 + 0xC)) {
            *(unsigned char *)(arg0 + 4) = 3;
            return;
        }
        {
            int i = 0;
            char *p = arg0;
            while (i < (int)*(unsigned char *)(arg0 + 0xC)) {
                *(int *)(p + 0x110) = func_001AF780();
                p += 4;
                i += 1;
            }
            *(unsigned char *)(arg0 + 9) = *(unsigned char *)(arg0 + 0xC);
        }
        anim_bone_array_setup(*(unsigned char *)(arg0 + 0xC));
        bone_init_default_1(arg0);
        {
            float jitter = 0.15f + (0.049999997f * (4.656613e-10f * (float)func_00122BB8()));
            *(float *)(arg0 + 0x68) = jitter;
            *(float *)(arg0 + 0x64) = jitter;
            *(float *)(arg0 + 0x60) = jitter;
        }
        *(float *)(step + 0x48) = 0.15f;
        *(float *)(step + 0x44) = 0.15f;
        *(float *)(step + 0x40) = 0.15f;
        sub = *(unsigned char *)(arg0 + 0xD);
        switch (sub) {
        case 3:
        case 4:
            *(float *)(arg0 + 0x88) = -96.0f;
            *(float *)(arg0 + 0x84) = -96.0f;
            *(float *)(arg0 + 0x80) = -96.0f;
            break;
        default:
            *(float *)(arg0 + 0x88) = 0.0f;
            *(float *)(arg0 + 0x84) = 0.0f;
            *(float *)(arg0 + 0x80) = 0.0f;
            break;
        }
        *(int *)(arg0 + 0x8C) = 0;
        func_001D80E0((int)(arg0 + 0x100), (int)&tmp);
        *(short *)(arg0 + 0x28) = 0;
        *(unsigned char *)(arg0 + 4) = 1;
        /* fallthrough */
    case 1:
        {
        short frame = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = frame + 1;

        switch (frame) {
        case 3:
            sub = *(unsigned char *)(arg0 + 0xD);
            switch (sub) {
            case 3:
            case 1:
            case 4:
            case 2:
                break;
            default:
                func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 7), 2);
                break;
            }
            break;
        case 0:
        case 1:
        case 2:
            sub = *(unsigned char *)(arg0 + 0xD);
            switch (sub) {
            case 0:
                func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 8), 2);
                /* fallthrough */
            case 1:
                func_001F4F90(arg0, 2.4f);
                break;
            case 3:
            case 4:
            case 2:
                break;
            }
            break;
        default: {
            float eased;
            *(float *)(arg0 + 0x80) += 0.35f * (-128.0f - *(float *)(arg0 + 0x80));
            eased = *(float *)(arg0 + 0x80);
            *(float *)(arg0 + 0x88) = eased;
            *(float *)(arg0 + 0x84) = eased;
            break;
        }
        case 0xF:
            *(unsigned char *)(arg0 + 4) = 2;
            break;
        }

        *(float *)(arg0 + 0x60) += *(float *)(step + 0x40);
        *(float *)(arg0 + 0x64) += *(float *)(step + 0x44);
        *(float *)(arg0 + 0x68) += *(float *)(step + 0x48);
        *(float *)(step + 0x40) *= 0.8f;
        *(float *)(step + 0x44) *= 0.8f;
        *(float *)(step + 0x48) *= 0.8f;
        func_001C63D0(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        break;
    case 2:
    case 3:
        func_001AFC10(arg0);
        return;
    }
}
