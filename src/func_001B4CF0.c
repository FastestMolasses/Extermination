// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

extern unsigned char **D_00275B40;
extern char D_70003600[];
extern char D_70003610[];

extern void func_00102948(char *, unsigned char *);
extern void func_001026A0(char *, unsigned char *, char *);
extern void func_001EFD90(int, char *, char *);
extern void func_001FBD50(unsigned char *, int, int, float);
extern float func_001B1470(float);

void func_001B4CF0(unsigned char *p) {
    int idx;
    int sid;

    switch (p[3]) {
    case 1:
        idx = 4;
        if (p[0xD] & 1) {
            if (p[0xD] & 0x80) {
                *(volatile float *)0x70003600 = 2.142f;
                *(volatile float *)0x70003604 = 1.968f;
                *(volatile float *)0x70003608 = -0.059f;
                *(volatile float *)0x7000360C = 1.0f;
            } else {
                *(volatile float *)0x70003600 = 2.152f;
                *(volatile float *)0x70003604 = 1.954f;
                *(volatile float *)0x70003608 = -0.049f;
                *(volatile float *)0x7000360C = 1.0f;
            }
        } else {
            if (p[0xD] & 0x80) {
                *(volatile float *)0x70003600 = 2.177f;
                *(volatile float *)0x70003604 = 1.896f;
                *(volatile float *)0x70003608 = 0.0f;
                *(volatile float *)0x7000360C = 1.0f;
            } else {
                *(volatile float *)0x70003600 = 2.06f;
                *(volatile float *)0x70003604 = 2.109f;
                *(volatile float *)0x70003608 = 0.0f;
                *(volatile float *)0x7000360C = 1.0f;
            }
        }
        if (p[0xD] & 0x80) {
            sid = 0x8000006F;
        } else {
            sid = 0x80000070;
        }
        func_00102948(D_70003610, p + 0xC0);
        break;
    case 3:
        sid = 0x80000070;
        *(volatile float *)0x70003600 = 1.089f;
        *(volatile float *)0x70003604 = 0.816f;
        *(volatile float *)0x70003608 = 0.0f;
        *(volatile float *)0x7000360C = 1.0f;
        idx = 2;
        func_00102948(D_70003610, p + 0xC0);
        break;
    case 4:
        if (p[0xD] & 0x80) {
            sid = 0x80000071;
        } else {
            sid = 0x80000070;
        }
        *(volatile float *)0x70003600 = 0.335f;
        *(volatile float *)0x70003604 = 0.569f;
        *(volatile float *)0x70003608 = 0.0f;
        *(volatile float *)0x7000360C = 1.0f;
        idx = 2;
        func_00102948(D_70003610, p + 0xC0);
        break;
    case 5:
        idx = 0xD;
        if (p[0xD] & 0x80) {
            sid = 0x80000071;
            *(volatile float *)0x70003600 = 2.389f;
            *(volatile float *)0x70003604 = -0.655f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        } else {
            sid = 0x80000070;
            *(volatile float *)0x70003600 = 2.389f;
            *(volatile float *)0x70003604 = -0.655f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        }
        func_00102948(D_70003610, p + 0xC0);
        break;
    case 6:
        idx = 5;
        if (p[0xD] & 0x80) {
            sid = 0x80000071;
            *(volatile float *)0x70003600 = -0.318f;
            *(volatile float *)0x70003604 = -2.732f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        } else {
            sid = 0x80000070;
            *(volatile float *)0x70003600 = 0.0f;
            *(volatile float *)0x70003604 = -2.025f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        }
        func_00102948(D_70003610, p + 0xC0);
        *(volatile float *)0x70003614 = func_001B1470(3.1415927f + *(volatile float *)0x70003614);
        break;
    case 7:
        idx = 0xB;
        if (p[0xD] & 0x80) {
            sid = 0x80000071;
            *(volatile float *)0x70003600 = 3.02f;
            *(volatile float *)0x70003604 = 0.44f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        } else {
            sid = 0x80000070;
            *(volatile float *)0x70003600 = 2.027f;
            *(volatile float *)0x70003604 = 0.0f;
            *(volatile float *)0x70003608 = 0.0f;
            *(volatile float *)0x7000360C = 1.0f;
        }
        func_00102948(D_70003610, p + 0xC0);
        break;
    case 9:
        sid = 0x8000006F;
        *(volatile float *)0x70003600 = 2.4293f;
        *(volatile float *)0x70003604 = -1.9016f;
        *(volatile float *)0x70003608 = -0.1725f;
        *(volatile float *)0x7000360C = 1.0f;
        idx = 0x1C;
        func_00102948(D_70003610, p + 0xC0);
        *(volatile float *)0x70003614 = func_001B1470(1.5707964f + *(volatile float *)0x70003614);
        break;
    case 10:
        sid = 0x80000058;
        *(volatile float *)0x70003600 = 29.812f;
        *(volatile float *)0x70003604 = -0.935f;
        *(volatile float *)0x70003608 = 0.0f;
        *(volatile float *)0x7000360C = 1.0f;
        idx = 0x10;
        func_00102948(D_70003610, p + 0xC0);
        *(volatile float *)0x70003614 = func_001B1470(3.1415927f + *(volatile float *)0x70003614);
        break;
    case 11:
        sid = 0x80000071;
        *(volatile float *)0x70003600 = 11.549f;
        *(volatile float *)0x70003604 = 16.735f;
        *(volatile float *)0x70003608 = -0.008f;
        *(volatile float *)0x7000360C = 1.0f;
        idx = 2;
        func_00102948(D_70003610, p + 0xC0);
        break;
    default:
        return;
    }
    func_001026A0(D_70003600, D_00275B40[idx] + 0x90, D_70003600);
    func_001EFD90(0x80000036, D_70003600, D_70003610);
    func_001EFD90(sid, D_70003600, D_70003610);
    func_001EFD90(0x80000025, D_70003600, D_70003610);
    func_001FBD50(p, 0x15C, 0, 300.0f);
    func_001FBD50(p, 0x15D, 0, 300.0f);
}
