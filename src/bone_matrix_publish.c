// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// bone_matrix_publish: drives the skeleton/animation pose for an object then
// publishes each bone's world matrix. If the object's type byte at +0x1F0 is
// 0x31 or 0x34, calls func_001749F0(obj, id, 0.0f, (float)objp[+0x276]); else
// passes 1.0f. Then func_001C6DA0(obj) (anim_eval_skeleton). Finally, for each
// of objp[0xC] bones, copies the qword matrix at (*node)+0x90 into dst[i] via
// func_00102958, where node = *(int *)((char *)D_00275B40 + i*4) — D_00275B40
// is a gp-rel (sdata) pointer to the bone-node pointer array.
//
// Previously parked at 96.23% (slt-into-branch + dead-lui-dup + saved-register
// allocation-order walls). mwcc 2.3.3 + the correct gp-rel sdata pointer
// addressing (-sdatathreshold 4, D_00275B40 as `int *`, indexing as
// *(int *)((char *)D_00275B40 + off)) resolves all three; objdiff 100.0.
typedef struct Mat4 { unsigned char _b[0x40]; } Mat4;
extern int *D_00275B40;
extern int func_001749F0(unsigned char *obj, int id, float a, float b);
extern void func_001C6DA0(unsigned char *obj);
extern void func_00102958(Mat4 *dst, Mat4 *src);

void bone_matrix_publish(unsigned char *objp, int id, Mat4 *dstp)
{
  int i;
  int off;
  Mat4 *dst = dstp;
  unsigned char *obj = objp;
  unsigned char type = objp[0x1F0];

  if (type == 0x31 || type == 0x34) {
    func_001749F0(obj, id, 0.0f, (float)*(short *)(obj + 0x276));
  } else {
    func_001749F0(obj, id, 0.0f, 1.0f);
  }
  func_001C6DA0(obj);

  for (i = 0, off = 0; i < obj[0xC]; i++) {
    func_00102958(dst, (Mat4 *)(*(int *)((char *)D_00275B40 + off) + 0x90));
    off += 4;
    dst++;
  }
}
