import clang.cindex
# import chevron
from clang.cindex import CursorKind, AccessSpecifier, StorageClass

class ReflAttribute:
    def __init__(self):
        self.need_refl = False
        self.force_no_refl = False

class Node:
    def __init__(self, name: str):
        self.name = name
        self.children: list[Node] = []
        self.attrs = ReflAttribute()
        
class NamespaceNode(Node):
    def merge(self, n: Node):
        assert isinstance(n, NamespaceNode)
        self.children += n.children
        
        name_map: dict[str, list[Node]] = {}
        for child in self.children:
            name = child.name
            if isinstance(child, NamespaceNode):
                name = name + '::namespace' 
            elif isinstance(child, ClassNode):
                name = name + '::class'
            else:
                print('not support type')
                assert False
            if name not in name_map:
                name_map[name] = [child]
            else:
                name_map[name].append(child)
                
        self.children.clear()
        
        for node_list in name_map.values():
            for node in node_list[1:]:
                if isinstance(node, NamespaceNode) or isinstance(node, ClassNode):
                    node_list[0].merge(node)
            self.children.append(node_list[0])

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
        
    def merge(self, n: Node):
        assert isinstance(n, ClassNode)
        self.public_fields += n.public_fields
        self.protected_fields += n.protected_fields
        self.private_fields += n.private_fields
        
        self.public_functions += n.public_functions
        self.protected_functions += n.protected_functions
        self.private_functions += n.private_functions
        
        
g_node_list: list[Node] = []

def parse_attributes(attr_str: str) -> ReflAttribute:
    prefix = 'nickel('
    if len(attr_str) < len(prefix) or (attr_str[0:len(prefix)] != prefix and attr_str[-1] != ')'):
        # not nickel engine attribute, ignore
        return ReflAttribute()
    
    content = attr_str[len(prefix):-1]
    attributes = str.split(content, ',')
    split_attrs = list(map(lambda x: x.strip(), attributes))
    refl_attrs = ReflAttribute()
    if 'refl' in split_attrs:
        refl_attrs.need_refl = True 
    if 'norefl' in split_attrs:
        refl_attrs.force_no_refl = False
    return refl_attrs

def transform_attributes_by_parent(attrs: ReflAttribute, parent_attrs: ReflAttribute|None) -> ReflAttribute:
    parent_attributes = parent_attrs if parent_attrs is not None else ReflAttribute()
    final_attrs = ReflAttribute()
    final_attrs.force_no_refl = attrs.force_no_refl or parent_attributes.force_no_refl
    final_attrs.need_refl = not final_attrs.force_no_refl and (attrs.need_refl or parent_attributes.need_refl)
    return final_attrs

def record_node(node: Node, parent: Node|None, global_record_table: list[Node]):
    if isinstance(node, NamespaceNode) and len(node.children) == 0:
        return
    
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
            

def parse_namespace(cursor: clang.cindex.Cursor, parent: Node|None) -> NamespaceNode:
    assert cursor.kind == CursorKind.NAMESPACE
    node = NamespaceNode(cursor.spelling)
    
    if node is not None:
        for child in cursor.get_children():
            recurse_visit_cursor(child, node)
        record_node(node, parent, g_node_list)
    return node

def parse_variable(cursor: clang.cindex.Cursor, parent: Node|None) -> VariableNode:
    assert cursor.kind == CursorKind.FIELD_DECL
    node = VariableNode(cursor.spelling)
    if isinstance(parent, ClassNode):
        node.is_member = True
        node.type = cursor.type.spelling
        node.is_static = cursor.storage_class == StorageClass.STATIC
        
    for child in cursor.get_children():
        if child.kind == CursorKind.ANNOTATE_ATTR:
            node.attrs = parse_attributes(child.spelling)
    node.attrs = transform_attributes_by_parent(node.attrs, None if parent is None else parent.attrs)
    return node

def parse_function(cursor: clang.cindex.Cursor, parent: Node) -> FunctionNode:
    assert cursor.kind == CursorKind.CXX_METHOD
    node = FunctionNode(cursor.spelling)
    node.return_type = cursor.result_type.spelling
    node.is_static = cursor.storage_class == StorageClass.STATIC
    node.declare_type = cursor.type.spelling
    for child in cursor.get_children():
        if child.kind == CursorKind.PARM_DECL:
            node.args_types.append(child.type.spelling)
        elif child.kind == CursorKind.ANNOTATE_ATTR:
            node.attrs = parse_attributes(child.spelling)
    node.attrs = transform_attributes_by_parent(node.attrs, None if parent is None else parent.attrs)
    return node

def parse_class(cursor: clang.cindex.Cursor, parent: Node|None) -> ClassNode:
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
    class_node.attrs = transform_attributes_by_parent(class_node.attrs, None if parent is None else parent.attrs)
    
    if not class_node.attrs.need_refl:
        return class_node
    
    for child in cursor.get_children():
        recurse_visit_cursor(child, class_node)

    record_node(class_node, parent, g_node_list)
    
    return class_node

def recurse_visit_cursor(cursor: clang.cindex.Cursor, parent: Node|None):
    node: Node|None = None
    print(cursor.spelling, " ", cursor.kind, " ", cursor.spelling)
    if cursor.kind == CursorKind.NAMESPACE:
        node = parse_namespace(cursor, parent)
    if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
        node = parse_class(cursor, parent)
        
    if node is None:
        for child in cursor.get_children():
            recurse_visit_cursor(child, node)

def parse_one_file(filename: str):
    index = clang.cindex.Index.create()
    tu = index.parse(filename, args=['-std=c++20', '-D_NICKEL_REFLECTION_', '-I..'])
    if tu is None:
        print('parsing %s failed!' % filename)
    print('parsing %s ...' % filename)
    recurse_visit_cursor(tu.cursor, None)

if __name__ == '__main__':
    parse_one_file('../nickel/physics/vehicle.hpp')
    for node in g_node_list:
        print(node.name)