#ifndef STANDALONE
#include <stdio.h>
#endif  // STANDALONE

#include <emscripten/emscripten.h>
#include <stdlib.h>

#include "parser.h"
#include "yyjson.h"

EMSCRIPTEN_KEEPALIVE
char* parse(char* xml) {
    char* json_str = NULL;
    size_t json_len = 0;
    yyjson_mut_doc* doc = NULL;
    yyjson_mut_val* root = NULL;

    if (!xml || *xml == '\0') {
        return NULL;
    }

    doc = yyjson_mut_doc_new(NULL);
    root = yyjson_mut_obj(doc);

    yyjson_mut_doc_set_root(doc, root);

    // Parse XML into tree structure
    _parse_xml(doc, root, xml);

    json_str = yyjson_mut_write(doc, 0, &json_len);

    yyjson_mut_doc_free(doc);

    return json_str;
}

#ifndef STANDALONE
int main() {
    char* ptr = parse(
        "<xml><child1>content1</child1><child2>content2</child2><emptyChild/><childNoContent></"
        "childNoContent></xml>"
    );
    printf("%s\n", ptr);
    free(ptr);
}
#endif  // STANDALONE
