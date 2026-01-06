#ifndef NO_ENTRY
#include <stdio.h>
#endif  // NO_ENTRY

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include <stdlib.h>

#include "core/parser.h"
#include "yyjson.h"

typedef struct _parse_res_t {
    char* json_ptr;
    size_t json_len;
} parse_res;

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
parse_res* parse(char* xml) {
    parse_res* res = (parse_res*)malloc(sizeof(parse_res));
    size_t json_len = 0;
    yyjson_mut_doc* doc = NULL;
    yyjson_mut_val* root = NULL;

    if (!xml || *xml == '\0') {
        return NULL;
    }

    doc = yyjson_mut_doc_new(NULL);
    root = yyjson_mut_obj(doc);

    yyjson_mut_doc_set_root(doc, root);

    _parse_xml(doc, root, xml);

    res->json_ptr = yyjson_mut_write(doc, YYJSON_WRITE_ALLOW_INVALID_UNICODE, &res->json_len);

    yyjson_mut_doc_free(doc);

    return res;
}

#ifndef NO_ENTRY
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <xml>\n", argv[0]);
        return -1;
    }
    parse_res* res = parse(argv[1]);
    printf("%s\n", res->json_ptr);
    free(res);
}
#endif  // NO_ENTRY
