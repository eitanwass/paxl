#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <ctype.h>

#define OPEN_BRACKET '<'
#define CLOSE_BRACKET '>'
#define BACK_SLASH '/'

/** Skip whitespace characters */
static inline char* skip_ws(char* p) {
    while (*p && isspace(*p)) {
        p++;
    }
    return p;
}

static inline char* skip_to_tag_end(char* p) {
    while(*p && *p != CLOSE_BRACKET) {
        p++;
    }
    if (*p == CLOSE_BRACKET) {
        p++;
    }
    return p;
}

#endif  // __HELPERS_H__
