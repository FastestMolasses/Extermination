// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// Conditionally arms an object's timer/animation (default 20.0f), runs a setup
// step, dispatches one of two handlers by the low bit of its return code, then
// resets a small-data record and notifies a follow-up routine.
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef float f32;

extern s32 func_001C7420(s8 *, s32, s32);
extern s32 func_001CA7B0(s8 *, f32);
extern s32 func_001CAAC0(s32, s8 *);
extern s32 func_001D3AC0(s32);
extern s32 func_001D3CE0(s32);
extern s32 func_001D8C20(s32);
extern s8 *D_00275670;
extern s32 D_00275B44;

void func_001CAE40(s8 *arg0, s32 arg1)
{
  f32 var_f12;
  s32 temp_v0;
  s8 *temp_s0;
  s8 *temp_v1;
  s8 *p;

  temp_v1 = *((s8 **) (arg0 + 0x44));
  if ((temp_v1 != 0) != 0U) {
    var_f12 = *((f32 *) (temp_v1 + 0x20));
  } else {
    var_f12 = 20.0f;
  }
  temp_v0 = func_001CA7B0(arg0 + 0xB0, var_f12);
  if (temp_v0 >= 0) {
    temp_s0 = *((s8 **) (D_00275670 + 0x1C));
    func_001D8C20(1);
    func_001C7420(arg0, 0x3F5, 3);
    if (temp_v0 != 0) {
      if (temp_v0 & 1) {
        func_001D3CE0(arg1);
      } else {
        func_001D3AC0(arg1);
      }
    } else {
      func_001D3AC0(arg1);
    }
    p = D_00275670;
    *((s8 *) ((*((s8 **) (p + 0x1C))) + 3)) = 0x60;
    *((s32 *) ((*((s8 **) (p + 0x1C))) + 4)) = 0;
    *((s16 *) ((*((s8 **) (p + 0x1C))) + 0)) = 0;
    *((s8 **) (p + 0x1C)) = (*((s8 **) (p + 0x1C))) + 0x10;
    func_001D8C20(0);
    func_001CAAC0(D_00275B44 + 0xB0, temp_s0);
  }
}
