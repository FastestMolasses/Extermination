// NEARMISS func_0011CE20  (vram 0x0011CE20, 0x94C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 75.73% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc strength-reduction / register-allocation wall. Full algorithm recovered: signed (prec-3)/8 with the movn/movz select, the four 20-entry stack arrays at sp+0x00/0x50/0xA0/0xF0, the FIR convolution, the base-256 limb carry loop with its cvt.w.s/mfc1/mtc1/cvt.s.w round-to-int idiom, the ldexp(...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_0026C538[];
extern float D_0026C548[];

extern float func_0011E148(int n, float x);
extern float func_0011DF98(float x);

int func_0011CE20(float *w, float *out, int prec, int nw, int mode, int *xin) {
    int d[20];
    float b[20];
    float c[20];
    float e[20];
    int nb, sh, n1, ns, last, top, ip, ret;
    int round, carry, cnt, any, j, k, m, v, hb;
    float acc, sum, q, scale, hi;

    nb = (prec - 3) / 8;
    if (nb < 0) {
        nb = 0;
    }
    sh = prec - (nb + 1) * 8;
    n1 = nw - 1;
    ns = D_0026C538[mode];
    last = n1 + ns;
    j = nb - n1;
    if (last >= 0) {
        for (k = 0; k <= last; k++, j++) {
            if (j < 0) {
                b[k] = 0.0f;
            } else {
                b[k] = (float)xin[j];
            }
        }
    }
    if (ns >= 0) {
        for (k = 0; k <= ns; k++) {
            sum = 0.0f;
            for (m = 0; m <= n1; m++) {
                sum += w[m] * b[n1 + k - m];
            }
            e[k] = sum;
        }
    }
    top = ns;

again:
    acc = e[top];
    if (top > 0) {
        for (k = 0, m = top; m > 0; m--, k++) {
            q = (float)(int)(acc * (1.0f / 256.0f));
            d[k] = (int)(acc - q * 256.0f);
            acc = e[m - 1] + q;
        }
    }

    acc = func_0011E148(sh, acc);
    round = 0;
    acc = acc - func_0011DF98(acc * 0.125f) * 8.0f;
    ip = (int)acc;
    acc = acc - (float)ip;
    if (sh > 0) {
        v = d[top - 1];
        hb = v >> (8 - sh);
        ip += hb;
        v -= hb << (8 - sh);
        round = v >> (7 - sh);
        d[top - 1] = v;
    } else if (sh == 0) {
        round = d[top - 1] >> 8;
    } else if (acc >= 0.5f) {
        round = 2;
    }

    if (round > 0) {
        ip++;
        carry = 0;
        if (top > 0) {
            for (k = 0, m = top; m != 0; m--, k++) {
                if (carry) {
                    d[k] = 0xFF - d[k];
                } else if (d[k] != 0) {
                    d[k] = 0x100 - d[k];
                    carry = 1;
                }
            }
        }
        if (sh > 0) {
            if (sh == 1) {
                d[top - 1] &= 0x7F;
            } else if (sh == 2) {
                d[top - 1] &= 0x3F;
            }
        }
        if (round == 2) {
            acc = 1.0f - acc;
            if (carry) {
                acc = acc - func_0011E148(sh, 1.0f);
            }
        }
    }

    if (acc == 0.0f) {
        any = 0;
        for (k = top - 1; k >= ns; k--) {
            any |= d[k];
        }
        if (any == 0) {
            cnt = 1;
            if (d[ns - 1] == 0) {
                do {
                    cnt++;
                } while (d[ns - cnt] == 0);
            }
            for (k = top + 1; k <= top + cnt; k++) {
                b[n1 + k] = (float)xin[nb + k];
                sum = 0.0f;
                for (m = 0; m <= n1; m++) {
                    sum += w[m] * b[n1 + k - m];
                }
                e[k] = sum;
            }
            top = top + cnt;
            goto again;
        }
    }

    if (acc == 0.0f) {
        do {
            top--;
            sh -= 8;
        } while (d[top] == 0);
    } else {
        acc = func_0011E148(-sh, acc);
        if (acc >= 256.0f) {
            q = (float)(int)(acc * (1.0f / 256.0f));
            d[top] = (int)(acc - q * 256.0f);
            d[top + 1] = (int)q;
            top++;
            sh += 8;
        } else {
            d[top] = (int)acc;
        }
    }
    ret = ip & 7;

    scale = func_0011E148(sh, 1.0f);
    for (k = top; k >= 0; k--) {
        e[k] = scale * (float)d[k];
        scale = scale * (1.0f / 256.0f);
    }

    for (k = top; k >= 0; k--) {
        sum = 0.0f;
        for (m = 0; m <= ns && m <= top - k; m++) {
            sum += D_0026C548[m] * e[k + m];
        }
        c[top - k] = sum;
    }

    if (mode < 3) {
        if (mode > 0) {
            hi = c[0];
            sum = 0.0f;
            for (k = top; k >= 0; k--) {
                sum += c[k];
            }
            out[0] = sum;
            if (round != 0) {
                out[0] = -sum;
            }
            sum = hi - sum;
            for (k = 1; k <= top; k++) {
                sum += c[k];
            }
            out[1] = sum;
            if (round != 0) {
                out[1] = -sum;
            }
        } else if (mode == 0) {
            sum = 0.0f;
            for (k = top; k >= 0; k--) {
                sum += c[k];
            }
            out[0] = sum;
            if (round != 0) {
                out[0] = -sum;
            }
        }
    } else if (mode == 3) {
        for (k = top; k > 0; k--) {
            hi = c[k - 1] + c[k];
            c[k] = c[k] + (c[k - 1] - hi);
            c[k - 1] = hi;
        }
        for (k = top; k >= 2; k--) {
            hi = c[k - 1] + c[k];
            c[k] = c[k] + (c[k - 1] - hi);
            c[k - 1] = hi;
        }
        sum = 0.0f;
        for (k = top; k >= 2; k--) {
            sum += c[k];
        }
        if (round != 0) {
            out[0] = -c[0];
            out[1] = -c[1];
            out[2] = -sum;
        } else {
            out[2] = sum;
            out[0] = c[0];
            out[1] = c[1];
        }
    }
    return ret;
}
