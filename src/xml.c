#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

EM_JS(int, create_js_object, (), {
    // Create a global map if it doesn't exist
    if (!globalThis.paxml_obj) globalThis.paxml_obj = {};
    let id = 0;
    globalThis.paxml_obj[id] = {};
    return id;
});

EM_JS(void, set_js_object_key, (int obj_id, const char* key, const char* value), {
    globalThis.paxml_obj[obj_id][UTF8ToString(key)] = UTF8ToString(value);
});

int paxml(char* xml) {
    // test
    int root_handle = create_js_object();
    char* tag_open_start = strchr(xml, '<') + 1;
    char* tag_open_end = strchr(tag_open_start, '>');
    *tag_open_end = '\0';
    set_js_object_key(root_handle, tag_open_start, "Fuck if I know");
    free(xml);
    return root_handle;
}
