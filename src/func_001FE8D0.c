// CFLAGS: -O4,p -sdatathreshold 4
/* Far from GP (>32KB): declare as 8-byte array to prevent gp_rel */
extern int D_00264E30[2];
extern int D_00264E34[2];
extern int D_00264E38[2];
/* Near GP: normal int (gp_rel ok) */
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C60;
extern int D_00275C64;
extern int D_00275C68;
extern int D_00275C6C;
extern int D_00275840;
extern int D_00275844;

void func_001FE8D0(void) {
    int v1 = -1;
    D_00264E30[0] = 0;
    D_00264E34[0] = 0;
    D_00275C58 = 0;
    D_00275C5C = 0;
    D_00264E38[0] = 0;
    D_00275C60 = 0;
    D_00275C64 = 0;
    D_00275C68 = 0;
    D_00275C6C = 0;
    D_00275840 = v1;
    D_00275844 = v1;
}
