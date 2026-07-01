// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Area-transition dispatcher. Computes key = (D_00810700<<8) | D_00810701
// (area/sub-area pair) and zeroes six gp-relative session globals
// (D_00275C18/1C/20/24/28/2C: pointer + counters used by the per-area
// tendril/grid effect systems). Dispatches by exact key to one of 16
// area_dispatch_offXXXX_stateYYYY handlers (first arg = the literal key,
// second arg = the area byte<<8); keys with no match fall through with no
// call. Finally walks D_00275C2C records of a D_00275C20 array (stride
// 0xA060, matching the tendril-mesh stride used elsewhere off D_00275C18),
// zeroing the two ints at +0x54/+0x58 of each record.
extern void area_dispatch_off0080_state0300(int key, int a1);
extern void area_dispatch_off01E0_state0D00(int key, int a1);
extern void area_dispatch_off0240_state0E00(int key, int a1);
extern void area_dispatch_off02C0_state0B00(int key, int a1);
extern void area_dispatch_off0400_state0200(int key, int a1);
extern void area_dispatch_off0470_state0700(int key, int a1);
extern void area_dispatch_off0490_state1400(int key, int a1);
extern void area_dispatch_off0550_state0100(int key, int a1);
extern void area_dispatch_off0610_state0400(int key, int a1);
extern void area_dispatch_off0680_state1100(int key, int a1);
extern void area_dispatch_off0CF0_state0800(int key, int a1);
extern void area_dispatch_off0D90_state0600(int key, int a1);
extern void area_dispatch_off0F20_state1000(int key, int a1);
extern void area_dispatch_off1900_state0000(int key, int a1);
extern void area_dispatch_off1BC0_state1300(int key, int a1);
extern void area_dispatch_off2C80_state1500(int key, int a1);

extern int D_00275C18;
extern int D_00275C1C;
extern unsigned char *D_00275C20;
extern int D_00275C24;
extern int D_00275C28;
extern int D_00275C2C;
extern unsigned char D_00810700[64];
extern unsigned char D_00810701[64];

void func_001E7780(void) {
    int key;
    int a1;
    int i;
    int off;

    a1 = D_00810700[0] << 8;
    D_00275C24 = 0;
    D_00275C28 = 0;
    D_00275C2C = 0;
    D_00275C18 = 0;
    D_00275C1C = 0;
    key = a1 + D_00810701[0];
    D_00275C20 = 0;

    switch (key) {
    case 0x0:
        area_dispatch_off1900_state0000(1, a1);
        break;
    case 0x1:
        area_dispatch_off1900_state0000(1, a1);
        break;
    case 0x2:
        area_dispatch_off1900_state0000(2, a1);
        break;
    case 0x100:
        area_dispatch_off0550_state0100(0x100, a1);
        break;
    case 0x101:
        area_dispatch_off0550_state0100(0x101, a1);
        break;
    case 0x200:
        area_dispatch_off0400_state0200(0x200, a1);
        break;
    case 0x201:
        area_dispatch_off0400_state0200(0x201, a1);
        break;
    case 0x202:
        area_dispatch_off0400_state0200(0x202, a1);
        break;
    case 0x300:
        area_dispatch_off0080_state0300(0x300, a1);
        break;
    case 0x301:
        area_dispatch_off0080_state0300(0x301, a1);
        break;
    case 0x400:
        area_dispatch_off0610_state0400(0x400, a1);
        break;
    case 0x401:
        area_dispatch_off0610_state0400(0x401, a1);
        break;
    case 0x600:
        area_dispatch_off0D90_state0600(0x600, a1);
        break;
    case 0x601:
        area_dispatch_off0D90_state0600(0x601, a1);
        break;
    case 0x700:
        area_dispatch_off0470_state0700(0x700, a1);
        break;
    case 0x701:
        area_dispatch_off0470_state0700(0x701, a1);
        break;
    case 0x702:
        area_dispatch_off0470_state0700(0x702, a1);
        break;
    case 0x703:
        area_dispatch_off0470_state0700(0x703, a1);
        break;
    case 0x704:
        area_dispatch_off0470_state0700(0x704, a1);
        break;
    case 0x800:
        area_dispatch_off0CF0_state0800(0x800, a1);
        break;
    case 0x801:
        area_dispatch_off0CF0_state0800(0x801, a1);
        break;
    case 0x802:
        area_dispatch_off0CF0_state0800(0x802, a1);
        break;
    case 0x803:
        area_dispatch_off0CF0_state0800(0x803, a1);
        break;
    case 0x804:
        area_dispatch_off0CF0_state0800(0x804, a1);
        break;
    case 0x805:
        area_dispatch_off0CF0_state0800(0x805, a1);
        break;
    case 0x806:
        area_dispatch_off0CF0_state0800(0x806, a1);
        break;
    case 0xB00:
        area_dispatch_off02C0_state0B00(0xB00, a1);
        break;
    case 0xD00:
        area_dispatch_off01E0_state0D00(0xD00, a1);
        break;
    case 0xE00:
        area_dispatch_off0240_state0E00(0xE00, a1);
        break;
    case 0xF00:
        area_dispatch_off0080_state0300(0xF00, a1);
        break;
    case 0xF01:
        area_dispatch_off0080_state0300(0xF01, a1);
        break;
    case 0x1000:
        area_dispatch_off0F20_state1000(0x1000, a1);
        break;
    case 0x1001:
        area_dispatch_off0F20_state1000(0x1001, a1);
        break;
    case 0x1100:
        area_dispatch_off0680_state1100(0x1100, a1);
        break;
    case 0x1200:
        area_dispatch_off0080_state0300(0x1200, a1);
        break;
    case 0x1300:
        area_dispatch_off1BC0_state1300(0x1300, a1);
        break;
    case 0x1301:
        area_dispatch_off1BC0_state1300(0x1301, a1);
        break;
    case 0x1400:
        area_dispatch_off0490_state1400(0x1400, a1);
        break;
    case 0x1500:
        area_dispatch_off2C80_state1500(0x1500, a1);
        break;
    case 0x1600:
        area_dispatch_off0080_state0300(0x1600, a1);
        break;
    }

    for (i = 0, off = 0; i < D_00275C2C; i++, off += 0xA060) {
        unsigned char *rec = D_00275C20 + off;
        *(int *)(rec + 0x54) = 0;
        *(int *)(rec + 0x58) = 0;
    }
}
