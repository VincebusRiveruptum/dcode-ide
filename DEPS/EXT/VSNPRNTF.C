/*
 * Simple vsnprintf implementation for compilers without it (e.g., Watcom).
 * Supports: %s, %c, %d, %u, %x, %X
 * Does NOT support floats, width/precision on all formats, or locale.
 */

#include "VSNPRNTF.H"

/* Convert unsigned integer to string */
static int utoa_base(unsigned long value, char *buf, int base, int uppercase)
{
    char tmp[32];
    int i;
    int len;
    char *digits;

    digits = uppercase ?
        "0123456789ABCDEF" :
        "0123456789abcdef";

    i = 0;
    do {
        tmp[i++] = digits[value % base];
        value /= base;
    } while (value && i < (int)sizeof(tmp));

    len = i;
    while (i > 0) {
        *buf++ = tmp[--i];
    }

    return len;
}

int vsnprintf(char *out, size_t size, const char *fmt, va_list ap)
{
    char *dst;
    char *end;
    int count;

    if (size == 0) {
        return 0;
    }

    dst = out;
    end = out + size - 1;
    count = 0;

    while (*fmt && dst < end) {

        if (*fmt != '%') {
            *dst++ = *fmt++;
            count++;
            continue;
        }

        fmt++; /* skip '%' */

        if (*fmt == '%') {
            *dst++ = '%';
            fmt++;
            count++;
            continue;
        }

        if (*fmt == 'c') {
            char c;
            c = (char)va_arg(ap, int);
            *dst++ = c;
            fmt++;
            count++;
            continue;
        }

        if (*fmt == 's') {
            const char *s;
            s = va_arg(ap, const char *);
            if (!s) {
                s = "(null)";
            }
            while (*s && dst < end) {
                *dst++ = *s++;
                count++;
            }
            fmt++;
            continue;
        }

        if (*fmt == 'd') {
            long v;
            unsigned long uv;
            int numLen;

            v = va_arg(ap, int);
            if (v < 0) {
                *dst++ = '-';
                count++;
                uv = (unsigned long)(-v);
            } else {
                uv = (unsigned long)v;
            }

            numLen = utoa_base(uv, dst, 10, 0);
            dst += numLen;
            count += numLen;
            fmt++;
            continue;
        }

        if (*fmt == 'u') {
            unsigned long v;
            int numLen;
            v = va_arg(ap, unsigned int);
            numLen = utoa_base(v, dst, 10, 0);
            dst += numLen;
            count += numLen;
            fmt++;
            continue;
        }

        if (*fmt == 'x' || *fmt == 'X') {
            unsigned long v;
            int upper;
            int numLen;

            v = va_arg(ap, unsigned int);
            upper = (*fmt == 'X');
            numLen = utoa_base(v, dst, 16, upper);
            dst += numLen;
            count += numLen;
            fmt++;
            continue;
        }

        /* Unknown specifier: print it literally */
        *dst++ = '%';
        count++;
    }

    *dst = '\0';
    return count;
}

int snprintf(char *out, size_t size, const char *fmt, ...)
{
    va_list ap;
    int result;

    va_start(ap, fmt);
    result = vsnprintf(out, size, fmt, ap);
    va_end(ap);

    return result;
}
