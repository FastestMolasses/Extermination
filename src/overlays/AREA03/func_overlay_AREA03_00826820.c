// CFLAGS: -O4,p -sdatathreshold 4
// Convert D_00275CA0 to !!boolean inverted, store back, return 1.
extern unsigned int D_00275CA0;

int func_overlay_AREA03_00826820(void) {
    D_00275CA0 = (D_00275CA0 != 0) ^ 1;
    return 1;
}
