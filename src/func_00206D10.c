// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Decode/finalize step for an MPEG/stream object at arg0:
//   func_00204250(arg0+0x48)  -- init/reset a sub-state at +0x48
//   func_00203B70(&D_007A55A0) -- act on a global stream handle
//   sub_sceMpegGetPicture_decode_error(arg0) -- kick off async decode
//   spin until the global busy flag D_007A55AC clears
//   func_00206B90(arg0, 3) -- finish with mode 3
extern void func_00204250(int);
extern void func_00203B70(void *);
extern void sub_sceMpegGetPicture_decode_error(int);
extern void func_00206B90(int, int);
extern int D_007A55A0;
extern volatile int D_007A55AC;

void func_00206D10(int arg0) {
    func_00204250(arg0 + 0x48);
    func_00203B70(&D_007A55A0);
    sub_sceMpegGetPicture_decode_error(arg0);
    while (D_007A55AC != 0) {
    }
    func_00206B90(arg0, 3);
}
