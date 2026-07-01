// NEARMISS func_001F5040  (vram 0x001F5040, 0x450 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring wall: mwcc233 assigns arg0 to s2/step to s0/frame to s1 while the target uses s0/s1/a2 respectively (a persistent GP register swap, same operations otherwise), plus a 128-bit D_0025AD70 load/store codegen difference (folded-offset lq/sq vs target's separate address-then-load). L...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 2

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
// fills the clean-store/delay-slot residuals differently.
typedef int s128 __attribute__((mode(TI)));
extern s128 D_0025AD70;
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

void func_001F5040(char *arg0) {
    s128 tmp = D_0025AD70;
    char *step;
    unsigned char state;
    unsigned char sub;

    step = arg0 + 0x1F0;
    state = *(unsigned char *)(arg0 + 4);
    switch (state) {
    case 0:
        sub = *(unsigned char *)(arg0 + 0xD);
        switch (sub) {
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
        default:
            func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 0xD), 2);
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
        if (sub == 4 || sub == 3) {
            *(float *)(arg0 + 0x88) = -96.0f;
            *(float *)(arg0 + 0x84) = -96.0f;
            *(float *)(arg0 + 0x80) = -96.0f;
        } else {
            *(float *)(arg0 + 0x88) = 0.0f;
            *(float *)(arg0 + 0x84) = 0.0f;
            *(float *)(arg0 + 0x80) = 0.0f;
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
        case 0xF:
            *(unsigned char *)(arg0 + 4) = 2;
            break;
        case 2:
        case 1:
        case 0:
            sub = *(unsigned char *)(arg0 + 0xD);
            switch (sub) {
            case 2:
            case 4:
            case 3:
                break;
            case 0:
                func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 8), frame);
                /* fallthrough */
            case 1:
                func_001F4F90(arg0, 2.4f);
                break;
            }
            break;
        case 3:
            sub = *(unsigned char *)(arg0 + 0xD);
            if (sub != 2 && sub != 4 && sub != 1 && sub != 3) {
                func_001CA5E0((int *)arg0, (int)func_001C6120((char *)D_0028A56C, 7), frame);
            }
            break;
        default: {
            float cur = *(float *)(arg0 + 0x80);
            float eased = cur + (0.35f * (-128.0f - cur));
            *(float *)(arg0 + 0x80) = eased;
            *(float *)(arg0 + 0x88) = eased;
            *(float *)(arg0 + 0x84) = eased;
            break;
        }
        }

        *(float *)(arg0 + 0x60) = *(float *)(arg0 + 0x60) + *(float *)(step + 0x40);
        *(float *)(arg0 + 0x64) = *(float *)(arg0 + 0x64) + *(float *)(step + 0x44);
        *(float *)(arg0 + 0x68) = *(float *)(arg0 + 0x68) + *(float *)(step + 0x48);
        *(float *)(step + 0x40) = *(float *)(step + 0x40) * 0.8f;
        *(float *)(step + 0x44) = *(float *)(step + 0x44) * 0.8f;
        *(float *)(step + 0x48) = *(float *)(step + 0x48) * 0.8f;
        func_001C63D0(arg0);
        func_001B17A0(arg0);
        (*(void (**)(char *))(arg0 + 0x4C))(arg0);
        }
        break;
    case 3:
    case 2:
        func_001AFC10(arg0);
        return;
    }
}
