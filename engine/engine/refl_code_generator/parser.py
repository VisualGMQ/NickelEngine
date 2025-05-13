import os
import sys
import clang.cindex
import chevron
from clang.cindex import CursorKind, AccessSpecifier, StorageClass


class Node:
    def __init__(self, name: str):
        self.name = name
        self.children = []
        
class NamespaceNode(Node):
    def merge(self, n: Node):
        assert isinstance(n, NamespaceNode)
        self.children += n.children

class VariableNode(Node):
    def __init__(self, name: str):
        super().__init__(name)
        self.type = None
        self.is_static = False
        self.is_member = False
        
class FunctionNode(Node):
    def __init__(self, name: str):
        super().__init__(name)
        self.args_types: list[str] = []
        self.return_type: str|None = None
        self.is_static: bool = False
        self.declare_type: str|None = None

class ClassNode(Node):
    def __init__(self, name: str):
        super().__init__(name)
        self.public_fields: list[VariableNode] = []
        self.protected_fields: list[VariableNode] = []
        self.private_fields: list[VariableNode] = []
        
        self.public_functions = []
        self.protected_functions = []
        self.private_functions = []
        
        self.attrs = []
        
    def merge(self, n: Node):
        assert isinstance(n, ClassNode)
        self.public_fields += n.public_fields
        self.protected_fields += n.protected_fields
        self.private_fields += n.private_fields
        
        self.public_functions += n.public_functions
        self.protected_functions += n.protected_functions
        self.private_functions += n.private_functions
        
        
g_node_list: list[Node] = []

def parse_attributes(attrs: str) -> list[str]:
    attributes = str.split(attrs, ',')
    return map(lambda x: x.strip, attributes)

def record_node(node: Node|None, parent: Node|None, global_record_table: list[Node]):
    if parent is not None:
        mach: Node = next((x for x in parent.children if x.name == node.name and type(x) == type(node)), None)
        if mach is None:
            parent.children.append(node)
        else:
            mach.merge(node)
    else:
        mach: Node = next((x for x in global_record_table if x.name == node.name and type(x) == type(node)), None)
        if mach is None:
            global_record_table.append(node)
        else:
            mach.merge(node)
            

def parse_namespace(cursor: clang.cindex.Cursor, parent: Node) -> NamespaceNode:
    assert cursor.kind == CursorKind.NAMESPACE
    return NamespaceNode(cursor.spelling)

def parse_variable(cursor: clang.cindex.Cursor, parent: Node) -> VariableNode:
    assert cursor.kind == CursorKind.FIELD_DECL
    node = VariableNode(cursor.spelling)
    if isinstance(parent, ClassNode):
        node.is_member = True
        node.type = cursor.type.spelling
        node.is_static = cursor.storage_class == StorageClass.STATIC
    return node

def parse_function(cursor: clang.cindex.Cursor, parent: Node) -> FunctionNode:
    assert cursor.kind == CursorKind.CXX_METHOD
    node = FunctionNode(cursor.spelling)
    node.return_type = cursor.result_type.spelling
    node.is_static = cursor.storage_class == StorageClass.STATIC
    node.declare_type = cursor.type.spelling
    for child in cursor.get_children():
        if child.kind != CursorKind.PARM_DECL:
            continue
        node.args_types.append(child.type.spelling)
    return node

def parse_class(cursor: clang.cindex.Cursor, parent: Node) -> ClassNode:
    assert cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL
    class_node = ClassNode(cursor.spelling)
    for child in cursor.get_children():
        if child.kind == CursorKind.FIELD_DECL:
            var_node = parse_variable(child, class_node)
            if child.access_specifier == AccessSpecifier.PUBLIC:
                class_node.public_fields.append(var_node)
            elif child.access_specifier == AccessSpecifier.PROTECTED:
                class_node.protected_fields.append(var_node)
            elif child.access_specifier == AccessSpecifier.PRIVATE:
                class_node.private_fields.append(var_node)
        elif child.kind == CursorKind.CXX_METHOD:
            fn_node = parse_function(child, class_node)
            if child.access_specifier == AccessSpecifier.PUBLIC:
                class_node.public_functions.append(fn_node)
            elif child.access_specifier == AccessSpecifier.PROTECTED:
                class_node.protected_functions.append(fn_node)
            elif child.access_specifier == AccessSpecifier.PRIVATE:
                class_node.private_functions.append(fn_node)
        elif child.kind == CursorKind.ANNOTATE_ATTR:
            class_node.attrs = parse_attributes(child.spelling)
    return class_node

def recurse_visit_cursor(cursor: clang.cindex.Cursor, parent: Node|None):
    node: Node|None = None
    # print(cursor.kind, " ", cursor.spelling)
    if cursor.kind == CursorKind.NAMESPACE:
        node = parse_namespace(cursor, parent)
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        node = parse_class(cursor, parent)
        
    if node is not None:
        record_node(node, parent, g_node_list)
    for child in cursor.get_children():
        recurse_visit_cursor(child, node)

def parse_one_file(filename: str):
    index = clang.cindex.Index.create()
    tu = index.parse(filename, args=['-std=c++20', '-D_NICKEL_REFLECTION_'])
    if tu is None:
        print('parsing %s failed!' % filename)
    print('parsing %s ...' % filename)
    recurse_visit_cursor(tu.cursor, None)

if __name__ == '__main__':
    parse_one_file('test/test.hpp')
    for node in g_node_list:
        print(node.name)