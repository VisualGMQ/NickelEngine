The standard way to compile `tree-sitter-cpp` is:

1. find `tree-sitter` CLI
2. use CLI to parse grammar files and generate new `src/parser.c`
3. compile 

But I don't want add `tree-sitter` CLI dependency, So I pre-generated a `src/parser.c` use version `0.25.2` CLI tool. When we upgrade `tree-sitter` or `tree-sitter-cpp` we need to re-generate this file!

