#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>

#include "parser.h"

EM_JS(int, create_js_object, (), {
    // Create a global map if it doesn't exist
    if (!globalThis.paxl_obj) globalThis.paxl_obj = {};
    let id = globalThis.paxl_obj_counter || 0;
    globalThis.paxl_obj_counter = id + 1;
    globalThis.paxl_obj[id] = [];
    return id;
});

EM_JS(int, create_js_child_object, (int parent_id, const char* tag_name), {
    let child_id = globalThis.paxl_obj_counter || 0;
    globalThis.paxl_obj_counter = child_id + 1;
    globalThis.paxl_obj[child_id] = {
        "tagName": UTF8ToString(tag_name),
        "attributes": {},
        "children": [],
    };
    
    if ("children" in globalThis.paxl_obj[parent_id]) {
        globalThis.paxl_obj[parent_id].children.push(globalThis.paxl_obj[child_id]);
    } else {
        globalThis.paxl_obj[parent_id].push(globalThis.paxl_obj[child_id]);
    }
    
    return child_id;
});

EM_JS(void, set_js_object_value_direct, (int obj_id, const char* value), {
    globalThis.paxl_obj[obj_id]["children"] = [UTF8ToString(value)];
});

// Recursively build JS objects from XmlNode tree
static void build_js_from_node(const XmlNode* node, int obj_id) {
    XmlNode* child = NULL;
    for (size_t i = 0; i < node->child_count; i++) {
        child = node->children[i];

        int child_id = create_js_child_object(obj_id, child->name);
        if (child->child_count == 0) {
            if (child->text) {
                set_js_object_value_direct(child_id, child->text);
            }
        } else {
            build_js_from_node(child, child_id);
        }
    }
}

// Parse XML string and return JS object handle
int parse(char* xml) {
    if (!xml || *xml == '\0') return -1;

    
    
    // Allocate root node
    XmlNode* root = (XmlNode*)malloc(sizeof(XmlNode));
    memset(root, 0, sizeof(XmlNode));
    
    // Parse XML into tree structure
    _parse(root, xml);
    
    // Create wrapper JS object
    int wrapper_id = create_js_object();
    
    // Assume root has one child, the root element
    build_js_from_node(root, wrapper_id);
    
    // Cleanup tree
    free_node(root);
    
    return wrapper_id;
}
