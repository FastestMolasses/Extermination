// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 87.50% NEARMISS).
// Fixes read from the original instructions:
// - The loop is a plain counted for-loop over buf[i] that returns i + 1 on a
//   newline or carriage return; the old next/do-while spelling coloured the
//   index and the buffer pointer into swapped registers.
// - D_00276E10 is a record whose word at +0x18 is the receive ring pointer;
//   the ring is the struct func_0010CA30 consumes (size, count, read ptr).
// - The busy-wait polls the ring pointer through its own split symbol
//   D_00276E28 (the same word as D_00276E10.ring), which pairs the two
//   high-half relocations the way the original object does.
//
// SDK TTY read: block until the receive ring holds a byte, copy it into
// buf, consume it, and stop after a line terminator or count bytes.
// Returns the number of bytes stored.

struct ring {
    /* 0x0 */ int size;
    /* 0x4 */ volatile int count;
    /* 0x8 */ char *rd;
};

struct tty_ctx {
    /* 0x00 */ int pad[6];
    /* 0x18 */ struct ring *ring;
};

extern struct tty_ctx D_00276E10;
extern struct ring *D_00276E28; /* D_00276E10.ring */
extern void func_0010CA30(struct ring *r);

int func_0010CD58(char *buf, int count) {
    int i;
    for (i = 0; i < count; i++) {
        while (D_00276E28->count == 0)
            ;
        buf[i] = *D_00276E10.ring->rd;
        func_0010CA30(D_00276E10.ring);
        if (buf[i] == '\n' || buf[i] == '\r')
            return i + 1;
    }
    return i;
}
