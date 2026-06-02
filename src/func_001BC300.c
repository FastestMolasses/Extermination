extern void func_001C68C0(void *self);
extern void func_001B1B30(void *self, float x, float y, float z);

// Refresh an entity then re-aim it: bias the stored Y by +10 and feed the
// (x, y+10, z) anchor to the placement helper, then run its per-frame method.
void func_001BC300(unsigned char *self) {
    void (*method)(void *);
    func_001C68C0(self);
    func_001B1B30(self,
                  *(float *)(self + 0xB0),
                  10.0f + *(float *)(self + 0xB4),
                  *(float *)(self + 0xB8));
    method = *(void (**)(void *))(self + 0x4C);
    method(self);
}
