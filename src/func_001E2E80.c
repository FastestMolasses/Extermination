// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame update for one "charging/rolling" enemy kind.
//   e   = entity block; b = e+0x1F0 = its per-kind behaviour block.
//   e[4]   = top-level state: 0 = init (falls through to 1), 1 = run,
//            2/3 = finished -> func_001AFC10(e) tears the actor down.
//   e[5]   = one-shot flag for the trailing "spin the local frame by pi" pass.
//   e[0xD] = enemy kind (3 or 4 select the two behaviour variants).
//   e+0x94 = short enemy id used to pick the damage amount from the two
//            9-entry jump tables (jtbl_0026E8C0 when the area byte D_0081070A
//            is 0, jtbl_0026E890 otherwise).
//   e+0xB0 = position, e+0xC0 = spawn/anchor point, e+0xD0 = orientation basis.
//   b+0x20 = animation phase (wraps at 2.0), b+0x24/0x28/0x2C = three per-spawn
//            random unit values, b+0x30 = per-frame advance (decays by 0.01),
//            b+0x34 = decay-delay counter, b+0x38 = lifetime ticks (150 -> 0).
//   0x700031D0 / 0x700031D4 = scratchpad pointers to the two collision results;
//   the hit target's byte 0 gets bit 1 set and its +0x22C receives the damage.
//
// MATCHING NOTE: func_001028D0 / func_00102918 MUST be prototyped `void *` and
// their pointer args passed WITHOUT an explicit (float *) cast. The implicit
// pointer conversion changes mwcc's call-argument materialization order: with
// float* prototypes mwcc splits the %hi/%lo pair of a non-final global argument
// and sinks the %lo addiu into the jal delay slot; with void* it keeps the pair
// adjacent and sinks the trailing reg+imm arg, which is what the target does.

extern int   func_00122BB8();
extern void  func_00102948(float *, float *);
extern void  func_00102900(float *, float *, float);
extern void  func_001028B8(float *, float *, float *);
extern void  func_001028D0(void *, void *, void *);
extern void  func_00102760(float *, float *);
extern void  func_001CD390(float *, float *);
extern int   func_0019A570(float *, float *, int, int);
extern int   func_0019AA80(float *, float *, int);
extern void  func_001EFD20(int, float *);
extern void  func_001031E0(float *, float *);
extern void  func_00102918(void *, void *, void *);
extern void  func_001EFEB0(int, float *);
extern void  func_001F02C0(float *, int, float);
extern void  func_001029C0(float *);
extern void  func_00102BB0(float *, float *, float);
extern void  func_001026D0(float *, float *, float *);
extern void  func_001D04B0(float *, int, void *, float, float);
extern void  func_001AFC10(unsigned char *);

extern unsigned char D_0081070A;
extern void  *D_00253740;
extern void  *D_002537D0;
extern void  *D_00253860;
extern void  *D_002538F0;
extern void  *D_00253980;
extern void  *D_00253A10;
extern float D_700031B0[];
extern float D_700036A0[];
extern float D_700036D0[];
extern float D_700038A0[];
extern float D_700038B0[];

void func_001E2E80(unsigned char *e)
{
    int hit;
    unsigned char *b;
    unsigned char *tgt;
    float amt;
    float t;
    float cv;
    int sub;
    short h;

    amt = 0.0f;
    hit = 0;
    b = e + 0x1F0;
    switch (e[4]) {
    case 0:
        *(int *)(b + 0x20) = 0;
        *(float *)(b + 0x24) = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(b + 0x28) = (float)func_00122BB8() / 2147483648.0f;
        *(float *)(b + 0x2C) = (float)func_00122BB8() / 2147483648.0f;
        *(int *)(b + 0x30) = 0;
        *(int *)(b + 0x34) = 0;
        *(int *)(b + 0x38) = 150;
        func_00102948((float *)(b + 0x10), (float *)(e + 0xB0));
        func_00102900((float *)b, (float *)(e + 0xC0), *(float *)(e + 0x20));
        e[4] = 1;
        e[5] = 0;
        /* fallthrough */
    case 1:
        func_001028B8((float *)(e + 0xB0), (float *)(e + 0xB0), (float *)b);
        *(float *)(e + 0xB4) += *(float *)(b + 0x30);
        switch (e[0xD]) {
        case 3:
            *(float *)(b + 0x30) += -0.00999999978f;
            break;
        case 4:
            *(int *)(b + 0x34) = *(int *)(b + 0x34) + 1;
            if (*(int *)(b + 0x34) > 25) {
                *(float *)(b + 0x30) += -0.00999999978f;
            }
            break;
        }
        *(int *)(b + 0x38) = *(int *)(b + 0x38) - 1;
        if (*(int *)(b + 0x38) < 0) {
            e[4] = 3;
        }
        func_001028D0(D_700038A0, b + 0x10, e + 0xB0);
        func_00102760(D_700038A0, D_700038A0);
        func_001CD390((float *)(e + 0xD0), D_700038A0);
        hit = hit | func_0019A570((float *)(b + 0x10), (float *)(e + 0xB0), 6, 0);
        if (hit != 0) {
            switch (*(unsigned char *)(*(volatile int *)0x700031D0 + 0x1A)) {
            case 0x32:
                hit = 0;
                break;
            }
        }
        if (hit == 0) {
            hit = hit | func_0019AA80((float *)(b + 0x10), (float *)(e + 0xB0), 0x20);
            if (hit != 0) {
                tgt = (unsigned char *)*(volatile int *)0x700031D4;
                if (tgt[0] == 1) {
                    if (D_0081070A == 0) {
                        switch (e[0xD]) {
                        case 3:
                            switch (*(short *)(e + 0x94)) {
                            case 10: amt = 10.0f; break;
                            case 11: amt = 12.0f; break;
                            case 12: amt = 20.0f; break;
                            case 13: amt = 25.0f; break;
                            case 14: amt = 15.0f; break;
                            case 15: amt = 25.0f; break;
                            case 16: amt = 20.0f; break;
                            case 17: amt = 20.0f; break;
                            case 18: amt = 25.0f; break;
                            }
                            break;
                        case 4:
                            h = *(short *)(e + 0x94);
                            switch (h) {
                            case 20: amt = 10.0f; break;
                            case 19: amt = 10.0f; break;
                            }
                            break;
                        }
                    } else {
                        switch (e[0xD]) {
                        case 3:
                            switch (*(short *)(e + 0x94)) {
                            case 10: amt = 15.0f; break;
                            case 11: amt = 17.0f; break;
                            case 12: amt = 20.0f; break;
                            case 13: amt = 30.0f; break;
                            case 14: amt = 20.0f; break;
                            case 15: amt = 25.0f; break;
                            case 16: amt = 25.0f; break;
                            case 17: amt = 30.0f; break;
                            case 18: amt = 25.0f; break;
                            }
                            break;
                        case 4:
                            h = *(short *)(e + 0x94);
                            switch (h) {
                            case 20: amt = 25.0f; break;
                            case 19: amt = 10.0f; break;
                            }
                            break;
                        }
                    }
                    if (amt != 0.0f) {
                        *(float *)(tgt + 0x22C) = amt;
                        tgt[0] = tgt[0] | 2;
                        func_001028D0(tgt + 0x70, e + 0xB0, b + 0x10);
                        func_00102760((float *)(tgt + 0x70), (float *)(tgt + 0x70));
                        func_001EFD20(0x8000001B, (float *)(e + 0xB0));
                    }
                }
            }
        }
        if (hit != 0) {
            func_001031E0(D_700038B0, D_700031B0);
            *(volatile int *)0x700038BC = 0x3F800000;
            func_00102918(D_700036A0, e + 0xD0, D_700038B0);
            func_001EFEB0(0x8000002A, D_700036A0);
            func_001F02C0(D_700036D0, 0x1B3, 500.0f);
            e[4] = 3;
        }
        sub = e[5];
        if (sub == 0) {
            e[5] = sub + 1;
            func_001029C0(D_700036A0);
            func_00102BB0(D_700036A0, D_700036A0, 3.14159274f);
            func_001026D0(D_700036A0, (float *)(e + 0xD0), D_700036A0);
            func_00102918(D_700036A0, D_700036A0, e + 0xB0);
            func_001EFEB0(0x80000029, D_700036A0);
        }
        func_00102918(e + 0xD0, e + 0xD0, e + 0xB0);
        switch (e[0xD]) {
        case 3:
            func_001D04B0((float *)(e + 0xD0), 1, &D_00253740, *(float *)(b + 0x20), *(float *)(b + 0x24));
            func_001D04B0((float *)(e + 0xD0), 1, &D_002537D0, *(float *)(b + 0x20), *(float *)(b + 0x28));
            func_001D04B0((float *)(e + 0xD0), 0, &D_00253860, *(float *)(b + 0x20), *(float *)(b + 0x2C));
            break;
        case 4:
            func_001D04B0((float *)(e + 0xD0), 1, &D_002538F0, *(float *)(b + 0x20), *(float *)(b + 0x24));
            func_001D04B0((float *)(e + 0xD0), 1, &D_00253980, *(float *)(b + 0x20), *(float *)(b + 0x28));
            func_001D04B0((float *)(e + 0xD0), 0, &D_00253A10, *(float *)(b + 0x20), *(float *)(b + 0x2C));
            break;
        }
        cv = 0.0500000007f;
        t = *(float *)(b + 0x20) + cv;
        *(float *)(b + 0x20) = t;
        if (t > 2.0f) {
            *(float *)(b + 0x20) = *(float *)(b + 0x20) - 1.0f;
        }
        func_00102948((float *)(b + 0x10), (float *)(e + 0xB0));
        break;
    case 2:
    case 3:
        func_001AFC10(e);
        break;
    }
}
