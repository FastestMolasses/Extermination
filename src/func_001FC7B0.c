// NEARMISS func_001FC7B0  (vram 0x001FC7B0, 0x200 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 61.16% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Whole-function register-allocation ORDER on a recursive text-wrap parser: structure, branch-likely char dispatch, 0x81-expansion, recursion and all call args recovered, but mwcc colors the four params to a shifted saved-reg set (target a0->s7/a1->s8(fp)/str->s6/cfg->s5) and the target spills one ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// SEMANTICS: recursive line-layout / text-wrap glyph emitter. arg0 = render base, arg1 = a parallel base,
// str = the text string, cfg = a layout-config struct (cfg[1]=glyph stride, cfg[2]/cfg[4] used for the
// newline line-advance (cfg[4]+cfg[2])>>1, cfg[5] = the draw param passed as func_001CC1E0's 7th arg).
// Copies a 0x80-byte glyph template from D_00264C10 into a local buffer (4x 0x20 quadword copies). len =
// strlen(str) (func_001232E0); returns immediately if empty. Walks the string char by char (index i):
//   * c == 0x0A (newline): recurse with arg1 advanced by (cfg[4]+cfg[2])>>1 onto str+i+1, then return.
//   * c < 0x20 (other control), c == 0x80, 0xA0<=c<0xE0, or c >= 0xF0: skip (i++).
//   * otherwise (a printable/glyph-run start: 0x20..0x9F except 0x80, or 0xE0..0xEF): start a word run.
//     base = arg0 + i*cfg[1]; memset the local buffer (func_00121A28(buf,0,0x80)); copy run chars into buf
//     while they stay in [0x20,0x100) and i<len, expanding a 0x81 lead byte into a space (0x20); then if any
//     chars were emitted, draw via func_001CC1E0(1, base+0x700, arg1+0x790, 0xA, 0x14, buf, cfg[5]).
//
// WALL: register-allocation ORDER. The structure, control flow, the branch-likely (bnel) char dispatch, the
// 0x81 expansion, the recursion and every call arg are recovered, but mwcc assigns the four params to a
// shifted set of saved registers (target a0->s7,a1->s8(fp),str->s6,cfg->s5; mwcc a0->s6,a1->s7,str->s5,...)
// and the target spills one extra callee-saved reg (fp), giving a 0x120 vs 0x110 frame. No C-level lever
// found to force the saved-reg coloring / extra-save; this is permuter territory on a recursive function.
extern void func_00121A28(void *dst, int val, int size);
extern unsigned int func_001232E0(unsigned char *s);
extern int func_001CC1E0(int a, int b, int c, int d, int e, char *f, int g);

extern char D_00264C10[];

typedef unsigned __int128 u128;

void func_001FC7B0(int arg0, int arg1, unsigned char *str, int *cfg) {
    char buf[0x80];
    char *dst;
    unsigned int len;
    int i;
    int n;
    int c;
    int base;

    {
        char *d = buf;
        char *s = D_00264C10;
        int k;
        for (k = 4; k > 0; k--) {
            u128 q0 = ((u128 *)s)[0];
            u128 q1 = ((u128 *)s)[1];
            ((u128 *)d)[0] = q0;
            ((u128 *)d)[1] = q1;
            s += 0x20;
            d += 0x20;
        }
    }

    len = func_001232E0(str);
    i = 0;
    if (len == 0) {
        return;
    }
    do {
        c = str[i];
        if (c < 0x20) {
            if (c == 0xA) {
                func_001FC7B0(arg0, arg1 + ((cfg[4] + cfg[2]) >> 1), str + i + 1, cfg);
                return;
            }
            i++;
        } else if (c == 0x80 || (c >= 0xA0 && c < 0xE0) || c >= 0xF0) {
            i++;
        } else {
            base = arg0 + i * cfg[1];
            func_00121A28(buf, 0, 0x80);
            dst = buf;
            n = 0;
            while ((c & 0xFF) >= 0x20 && (c & 0xFF) < 0x100 && i < len) {
                if ((c & 0xFF) == 0x81) {
                    i++;
                    c = 0x20;
                }
                i++;
                *dst++ = c;
                c = str[i];
                n++;
            }
            if (n > 0) {
                func_001CC1E0(1, base + 0x700, arg1 + 0x790, 0xA, 0x14, buf, cfg[5]);
            }
        }
    } while (i < len);
}
