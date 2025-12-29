#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <ctype.h>

/** Skip whitespace characters */
static inline char* skip_ws(char* p) {
    while (*p && isspace(*p)) {
        p++;
    }
    return p;
}

#endif  // __HELPERS_H__
