#include "tree_sitter/api.h"
#include "tree-sitter-cpp.h"

int main(int argc, char **argv) {
    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_cpp());

    ts_parser_delete(parser);
    return 0;
}