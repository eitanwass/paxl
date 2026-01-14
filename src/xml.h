#ifndef __XML_H__
#define __XML_H__

typedef struct _parse_res_t {
    char* json_ptr;
    size_t json_len;
} parse_res;

parse_res* parse(char* xml);

#endif  // __XML_H__
