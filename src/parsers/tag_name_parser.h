#ifndef __TAG_NAME_PARSER_H__
#define __TAG_NAME_PARSER_H__

#include "helpers.h"

/**
 * @brief Helper: Parse a tag name at `start` ptr
 * 
 * @param start     [IN]    A ptr to the start location of the tag name
 * @param len_out   [OUT]   Length of the tag name starting from `start`
 * @return char*            A ptr to the end of the tag name
 */
inline static char* parse_tag_name(char* start, size_t* len_out) {
    char* p = start;
    while (*p && *p != CLOSE_BRACKET && *p != BACK_SLASH && !isspace(*p)) {
        p++;
    }
    *len_out = p - start;
    return p;
}

#endif  // __TAG_NAME_PARSER_H__
