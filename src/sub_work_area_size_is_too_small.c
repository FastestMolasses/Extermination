// COMPILER: eegcc
// CFLAGS: -O2

extern void func_0010A3A8(const char *msg);
extern const char D_0026B5E8[];

unsigned int sub_work_area_size_is_too_small(unsigned int *heap, unsigned int size, unsigned int align)
{
    unsigned int cur;
    unsigned int end;

    cur = (heap[2] + align - 1) / align * align;
    end = cur + size;
    if (heap[0] + heap[1] >= end) {
        heap[2] = end;
        return cur;
    }
    func_0010A3A8(D_0026B5E8);
    return 0;
}
