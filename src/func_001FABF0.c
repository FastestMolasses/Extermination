// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.75% NEARMISS).
// Fixes read from the original instructions:
// - func_001FA790 takes (lane, clip) (its definition in src/ says so); the caller
//   passes its own first two arguments through unchanged. Declaring it (void)
//   changed the register pressure and flipped which parameter was copied to a
//   saved register before the enable test.
// - The flag check is a plain if / else-if with early returns (no goto into a
//   shared tail), and the final constant store is an if/else, the same spelling
//   as the byte-matched twin func_001FAD70; this keeps the original's dead
//   constant load after the lane-nonzero store and the epilogue offsets.
//
// Lane (re)arm / scale setter. If enable is set and lane idx is not yet active
// (D_00282154[idx] == 0), clears the lane's word at D_00282028, activates the
// lane via func_001FA790(idx, clip) and passes the lane's 64-bit handle at
// D_00281FD8 to func_0011A608; if enable is clear the lane must already be
// active. Then clears the lane's byte at D_0028202C and stores the lane's
// scale factor at D_00282024: 16383.0f / scale, or 16383.0f (0x467FFC00) when
// scale is 0. Lane records are 0x60 bytes apart. The lane-zero and lane-nonzero
// arms are identical in the original too.

extern char D_00282154[];
extern char D_00282028[];
extern char D_00281FD8[];
extern char D_0028202C[];
extern char D_00282024[];

extern void func_001FA790(int, int);
extern void func_0011A608(long long a, int b, int c);

void func_001FABF0(int idx, int a1, int scale, int enable) {
    int off;


    if (enable != 0) {
        if (D_00282154[idx] != 0) {
            return;
        }
        off = idx * 0x60;
        *(int *)(D_00282028 + off) = 0;
        func_001FA790(idx, a1);
        func_0011A608(*(long long *)(D_00281FD8 + off), 0, 0);
    } else if (D_00282154[idx] == 0) {
        return;
    }
    off = idx * 0x60;
    *(char *)(D_0028202C + off) = 0;
    if (scale != 0) {
        if (idx != 0) {
            *(float *)(D_00282024 + off) = 16383.0f / (float)scale;
            return;
        }
        *(float *)(D_00282024 + off) = 16383.0f / (float)scale;
        return;
    }
    if (idx != 0) {
        *(int *)(D_00282024 + off) = 0x467FFC00;
    } else {
        *(int *)(D_00282024 + off) = 0x467FFC00;
    }
}
