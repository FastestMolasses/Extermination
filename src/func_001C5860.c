// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Maps (100.0 - D_008104D8) into a 0..5 band via descending float thresholds
// (>80->0, >50->1, >30->2, >10->3, >0->4, else 5). Flat early-return cascade
// reproduces CW's per-check bc1t-next / b-end branch lowering; mwcc233/24 match.
extern float D_008104D8;

int func_001C5860(void) {
    float f = 100.0f - D_008104D8;
    if (f > 80.0f) return 0;
    if (f > 50.0f) return 1;
    if (f > 30.0f) return 2;
    if (f > 10.0f) return 3;
    if (f > 0.0f) return 4;
    return 5;
}
