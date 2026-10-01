// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Message bank lookup (MESSAGE_SERVICE.md). bank+0 is the offset of the
// directory, and the directory's +8 the offset of the string table (relative
// to the directory). The table starts with its base offset word, followed by
// 16-byte entries whose first word is each string's offset; returns
// table + table[0] + entry[idx].offset.
char *func_001FE480(char *bank, int idx) {
    char *tab = (char *)(*(int *)(bank + 8) + (int)(bank + *(int *)bank));
    int *e = (int *)(tab + 0x10);

    return tab + *(int *)tab + e[idx * 4];
}
