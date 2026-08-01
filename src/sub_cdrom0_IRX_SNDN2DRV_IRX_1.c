// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// SEMANTICS: IOP reboot + sound/driver IRX bring-up, run once at boot.
// Sequence: RPC/SIF preamble (func_0010E088(0), func_00112F98(0),
// func_001138D8(2)); spin until the IOP reboot request with the image path at
// D_0026DB70 is accepted (func_00110508); spin until the IOP reports back in
// sync (func_001104C0); redo the RPC preamble against the freshly booted IOP;
// reset the loadfile/heap services (func_0010F140, func_0010F870); then load
// six IRX modules from the path table D_0026DB90/DBB0/DBD0/DBF0/DC10/DC30
// (0x20 bytes apart -- the unit is named after cdrom0:\IRX\SNDN2DRV.IRX, the
// N2 sound driver), each retried until func_00110028(path, 0, 0) stops
// returning a negative error. Finally allocate the audio work buffers via
// func_001FA6A0: one 0x28000-byte block into D_00275B50 and three 0x10000-byte
// blocks into D_00275B28 (re-read and mirrored into D_00275B4C), D_00275B24
// and D_00275B20. Always returns 0.
//
// MATCH NOTE (s85 datum): mwcc 2.3.3 aligns loop-head branch targets to 8 bytes
// and emits the padding nops that 991202 omits -- 991202 caps at 84.0% purely
// on those missing nops. This is a NEW use for the 2.3.3 lane beyond the
// clean-store delay-slot case: the "CW branch-target alignment nop" wall is NOT
// a wall under 2.3.3.

extern void func_0010E088(int mode);
extern void func_00112F98(int mode);
extern void func_001138D8(int mode);
extern int func_00110508(char *path);
extern int func_001104C0(void);
extern void func_0010F140(void);
extern void func_0010F870(void);
extern int func_00110028(char *path, int argc, char *argv);
extern void *func_001FA6A0(int size);

extern char D_0026DB70[];
extern char D_0026DB90[];
extern char D_0026DBB0[];
extern char D_0026DBD0[];
extern char D_0026DBF0[];
extern char D_0026DC10[];
extern char D_0026DC30[];

extern void *D_00275B20;
extern void *D_00275B24;
extern void *D_00275B28;
extern void *D_00275B4C;
extern void *D_00275B50;

int sub_cdrom0_IRX_SNDN2DRV_IRX_1(void) {
    func_0010E088(0);
    func_00112F98(0);
    func_001138D8(2);
    while (func_00110508(D_0026DB70) == 0) {
    }
    while (func_001104C0() == 0) {
    }
    func_0010E088(0);
    func_00112F98(0);
    func_001138D8(2);
    func_0010F140();
    func_0010F870();
    while (func_00110028(D_0026DB90, 0, 0) < 0) {
    }
    while (func_00110028(D_0026DBB0, 0, 0) < 0) {
    }
    while (func_00110028(D_0026DBD0, 0, 0) < 0) {
    }
    while (func_00110028(D_0026DBF0, 0, 0) < 0) {
    }
    while (func_00110028(D_0026DC10, 0, 0) < 0) {
    }
    while (func_00110028(D_0026DC30, 0, 0) < 0) {
    }
    D_00275B50 = func_001FA6A0(0x28000);
    D_00275B28 = func_001FA6A0(0x10000);
    D_00275B4C = D_00275B28;
    D_00275B24 = func_001FA6A0(0x10000);
    D_00275B20 = func_001FA6A0(0x10000);
    return 0;
}
