import sys
import pickle
import os
from winreg import REG_DWORD_LITTLE_ENDIAN

import chevron
import pathlib
import clang.cindex
from clang.cindex import CursorKind, AccessSpecifier, StorageClass, TranslationUnitLoadError


class ReflAttribute:
    def __init__(self):
        self.need_refl = False
        self.force_no_refl = False
        self.need_script_register = False
        self.force_no_script_register  = False

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
            elif isinstance(child, EnumNode):
                name = name + '::enum'
            else:
                print("unsupport node type", flush=True)
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

        self.public_enums = []
        self.protected_enums = []
        self.private_enums = []
        
    def merge(self, n: Node):
        assert isinstance(n, ClassNode)
        self.public_fields += n.public_fields
        self.protected_fields += n.protected_fields
        self.private_fields += n.private_fields
        
        self.public_functions += n.public_functions
        self.protected_functions += n.protected_functions
        self.private_functions += n.private_functions
        
class EnumNode(Node):
    def __init__(self, name: str):
        super().__init__(name)
        self.items: list[str] = []
        
        
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
    if 'script' in split_attrs:
        refl_attrs.need_script_register = True 
    if 'norefl' in split_attrs:
        refl_attrs.force_no_refl = True
    if 'noscript' in split_attrs:
        refl_attrs.force_no_script_register = True
    return refl_attrs

def transform_attributes_by_parent(attrs: ReflAttribute, parent_attrs: ReflAttribute|None) -> ReflAttribute:
    parent_attributes = parent_attrs if parent_attrs is not None else ReflAttribute()
    final_attrs = ReflAttribute()
    final_attrs.force_no_refl = attrs.force_no_refl or parent_attributes.force_no_refl
    final_attrs.need_refl = not final_attrs.force_no_refl and (attrs.need_refl or parent_attributes.need_refl)
    final_attrs.force_no_script_register = attrs.force_no_script_register or parent_attributes.force_no_script_register
    final_attrs.need_script_register = not final_attrs.force_no_script_register and (attrs.need_script_register or parent_attributes.need_script_register)
    return final_attrs

def record_node(node: Node, parent: Node):
    if isinstance(node, NamespaceNode) and len(node.children) == 0:
        return
    
    mach: Node = next((x for x in parent.children if x.name == node.name and type(x) == type(node)), None)
    if mach is None:
        parent.children.append(node)
    else:
        mach.merge(node)

def parse_namespace(cursor: clang.cindex.Cursor, parent: Node, parsed_filename: pathlib.Path) -> NamespaceNode:
    assert cursor.kind == CursorKind.NAMESPACE
    node = NamespaceNode(cursor.spelling)
    
    if node is not None:
        for child in cursor.get_children():
            recurse_visit_cursor(child, node, parsed_filename)
        record_node(node, parent)
    return node

def parse_variable(cursor: clang.cindex.Cursor, parent: Node) -> VariableNode:
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
        elif child.kind == CursorKind.ENUM_DECL:
            enum_node = parse_enum(child, class_node)
            if child.access_specifier == AccessSpecifier.PUBLIC:
                class_node.public_enums.append(enum_node)
            elif child.access_specifier == AccessSpecifier.PROTECTED:
                class_node.protected_enums.append(enum_node)
            elif child.access_specifier == AccessSpecifier.PRIVATE:
                class_node.private_enums.append(enum_node)
        elif child.kind == CursorKind.ANNOTATE_ATTR:
            class_node.attrs = parse_attributes(child.spelling)
        elif child.kind == CursorKind.CLASS_DECL:
            parse_class(child, class_node)
    class_node.attrs = transform_attributes_by_parent(class_node.attrs, None if parent is None else parent.attrs)
    
    if not class_node.attrs.need_refl:
        return class_node
    
    record_node(class_node, parent)
    
    return class_node

def parse_enum(cursor: clang.cindex.Cursor, parent: Node) -> EnumNode:
    assert cursor.kind == CursorKind.ENUM_DECL
    enum_node = EnumNode(cursor.spelling)
    
    for child in cursor.get_children():
        if child.kind == CursorKind.ENUM_CONSTANT_DECL:
            enum_node.items.append(child.spelling)
        elif child.kind == CursorKind.ANNOTATE_ATTR:
            enum_node.attrs = parse_attributes(child.spelling)

    if enum_node.attrs.need_refl:
        record_node(enum_node, parent)
    return enum_node

def recurse_visit_cursor(cursor: clang.cindex.Cursor, parent: Node, parsed_filename: pathlib.Path):
    node: Node|None = None
    # print(cursor.spelling, " ", cursor.kind, " ", cursor.spelling)
    current_parsing_filename = pathlib.Path(cursor.location.file.name) if cursor.location.file else '' 
    if not (cursor.location.is_in_system_header or current_parsing_filename != parsed_filename):
        if cursor.kind == CursorKind.NAMESPACE:
            node = parse_namespace(cursor, parent, parsed_filename)
        if cursor.kind == CursorKind.STRUCT_DECL or cursor.kind == CursorKind.CLASS_DECL:
            node = parse_class(cursor, parent)
        if cursor.kind == CursorKind.ENUM_DECL:
            node = parse_enum(cursor, parent)
        
    if node is None:
        for child in cursor.get_children():
            recurse_visit_cursor(child, parent, parsed_filename)

def parse_one_file(filename: pathlib.Path, include_dir: str) -> Node:
    print(f'parsing {filename}', flush=True)
    index = clang.cindex.Index.create()
    root_node = Node("")
    
    content = filename.read_text(encoding='utf-8')
    if not content.find('NICKEL_REFL_ATTR'):
        return root_node
    
    try:
        tu = index.parse(str(filename), args=['-std=c++20', '-D_NICKEL_REFLECTION_', '-I' + include_dir])
    except TranslationUnitLoadError as e:
        print(f'parsing {filename} failed!', flush=True)
        return root_node
    recurse_visit_cursor(tu.cursor, root_node, filename)
    return root_node
   
g_class_refl_mustache = pathlib.Path('./mustache/refl/class_refl.mustache').read_text(encoding='utf-8')
g_enum_refl_mustache = pathlib.Path('./mustache/refl/enum_refl.mustache').read_text(encoding='utf-8')
g_refl_mustache = pathlib.Path('./mustache/refl/refl.mustache').read_text(encoding='utf-8')
g_refl_header_mustache = pathlib.Path('./mustache/refl/header.mustache').read_text(encoding='utf-8')
g_refl_impl_mustache = pathlib.Path('./mustache/refl/impl.mustache').read_text(encoding='utf-8')
 
g_class_script_binding_mustache = pathlib.Path('./mustache/script/class_binding.mustache').read_text(encoding='utf-8')
g_enum_script_binding_mustache = pathlib.Path('./mustache/script/enum_binding.mustache').read_text(encoding='utf-8')
g_script_binding_mustache = pathlib.Path('./mustache/script/binding.mustache').read_text(encoding='utf-8')
g_script_binding_header_mustache = pathlib.Path('./mustache/script/header.mustache').read_text(encoding='utf-8')
g_script_binding_impl_mustache = pathlib.Path('./mustache/script/impl.mustache').read_text(encoding='utf-8')


def node_code_generate(parsed_filename: str, node: Node) -> (str, str, str):
    final_filename = (parsed_filename.replace('\\', '/')
                      .replace('../', '')
                      .replace('./', ''))
    func_name = final_filename[:final_filename.find('.')]
    func_name = func_name.replace('/', '_')

    refl_fmt = {'parsed_filename': final_filename,
                'func_name': func_name,
                'enums': [], 'classes': []}
    binding_fmt = {'parsed_filename': final_filename,
                   'func_name': func_name,
                   'enums': [], 'classes': []}
    for child in node.children:
        node_code_generate_recursive('', child, refl_fmt, binding_fmt)

    return func_name, chevron.render(g_refl_mustache, refl_fmt), chevron.render(g_script_binding_mustache, binding_fmt)

    
def node_code_generate_recursive(prefix: str, node: Node, refl_out_fmt: dict[str, list], binding_out_fmt: dict[str, list]):
    new_prefix  = prefix + '::' + node.name
    
    # first generate enum, then class content
    if isinstance(node, EnumNode):
        refl_code, binding_code = enum_node_code_generate(new_prefix, node)
        refl_out_fmt['enums'].append({'enum': refl_code})
        binding_out_fmt['enums'].append({'enum': binding_code})

    if isinstance(node, ClassNode):
        for child in node.public_enums:
            refl_code, binding_code = enum_node_code_generate(new_prefix + '::' + child.name, child)
            refl_code = os.linesep + refl_code
            binding_code = os.linesep + binding_code
            refl_out_fmt['enums'].append({'enum': refl_code})
            binding_out_fmt['enums'].append({'enum': binding_code})

    for child in node.children:
        node_code_generate_recursive(new_prefix, child, refl_out_fmt, binding_out_fmt)
        
    if isinstance(node, ClassNode):
        refl_code, binding_code = class_node_code_generate(prefix, node)
        refl_code = os.linesep + refl_code
        binding_code = os.linesep + binding_code
        refl_out_fmt['classes'].append({'class': refl_code})
        binding_out_fmt['classes'].append({'class': binding_code})

def class_node_code_generate(prefix: str, node: ClassNode) -> (str, str):
    class_name_with_prefix = prefix + '::' + node.name
    refl_fmt = {'class_name': class_name_with_prefix,
                'class_register_name': node.name,
                'properties': []}
    binding_fmt = {'class_type': class_name_with_prefix,
                   'class_name': node.name,
                   'class_properties': []}
    for field in node.public_fields:
        if field.attrs.need_refl:
            refl_fmt['properties'].append({'property_register_name': field.name if len(field.name) <= 2 else field.name[2:],
                                           'property_name': class_name_with_prefix + '::' + field.name})

        if field.attrs.need_script_register:
            binding_fmt['class_properties'].append({'property_name': field.name if len(field.name) <= 2 else field.name[2:],
                                                    'property_type': class_name_with_prefix + '::' + field.name})

    refl_code = ""
    binding_code = ""
    if node.attrs.need_refl:
        refl_code = chevron.render(g_class_refl_mustache, refl_fmt)
    if node.attrs.need_script_register:
        binding_code = chevron.render(g_class_script_binding_mustache, binding_fmt)
    return refl_code, binding_code

def enum_node_code_generate(enum_name_with_prefix: str, node: EnumNode) -> (str, str):
    binding_code = ""
    refl_code = ""

    refl_fmt = {'enum_name': enum_name_with_prefix,
                'enum_register_name': node.name,
                'enums': []}
    binding_fmt = {'enum_type': enum_name_with_prefix,
                   'enum_name': node.name,
                   'enums': []}
    for item in node.items:
        if node.attrs.need_refl:
            refl_fmt['enums'].append({'enum_register_name': item,
                                    'enum_name': enum_name_with_prefix + '::' + item})
        if node.attrs.need_script_register:
            binding_fmt['enums'].append({'enum_item_name': item,
                                        'enum_item_value': enum_name_with_prefix + '::' + item})

    if node.attrs.need_refl:
        refl_code = chevron.render(g_enum_refl_mustache, refl_fmt)

    if node.attrs.need_script_register:
        binding_code = chevron.render(g_enum_script_binding_mustache, binding_fmt)

    return refl_code, binding_code

def save_generated_code(filename: str, code: str):
    with open(filename, 'w', encoding='utf-8') as f:
        f.write(code)
        
        

class NodeRecords:
    def __init__(self):
        self.mtime: dict[pathlib.Path, float] = {}
        self.parsed_file_record: dict[pathlib.Path, Node] = {}

if __name__ == '__main__':
    debug_mode = False
    if 'debug_mode' in sys.argv[1:]:
        debug_mode = True
    
    if not debug_mode and len(sys.argv) != 5:
        print('usage: parser.py parse_dir output_dir', flush=True)
        raise RuntimeError('invalid parameter')
    
    root_path = None
    parse_dir = None
    time_record_output = None
    refl_output_dir = None
    binding_output_dir = None
   
    if debug_mode:
        root_path = pathlib.Path(os.getcwd())
        parse_dir = pathlib.Path('test')
        time_record_output_dir = pathlib.Path('./')
        refl_output_dir = pathlib.Path('./refl')
        binding_output_dir = pathlib.Path('./binding')
    else:
        root_path = pathlib.Path(os.getcwd())
        parse_dir = pathlib.Path(sys.argv[1])
        time_record_output_dir = pathlib.Path(sys.argv[2])
        refl_output_dir = pathlib.Path(sys.argv[3])
        binding_output_dir = pathlib.Path(sys.argv[4])

    time_record_filename = 'time_record.pkl'
    time_record_file_path = time_record_output_dir / time_record_filename
    
    refl_header_filename = refl_output_dir / 'refl_generate.hpp'
    refl_impl_filename = refl_output_dir / 'refl_generate.cpp'

    binding_header_filename = binding_output_dir / 'script_binding.hpp'
    binding_impl_filename = binding_output_dir / 'script_binding.cpp'

    print(f'parse dir: {parse_dir}', flush=True)
    print(f'refl output dir: {refl_output_dir}', flush=True)
    print(f'binding output dir: {binding_output_dir}', flush=True)
    
    c_cpp_header_file_extensions = ['h', 'hpp', 'hxx']
    
    files: list[pathlib.Path] = []
    for ext in c_cpp_header_file_extensions:
        headers = list(parse_dir.glob('**/*.' + ext))
        if files is not None:
            files += headers

    # for debug
    # files = [pathlib.Path('../nickel/physics/vehicle.hpp')]

    file_record = NodeRecords()
    new_file_record = NodeRecords()
    
    if time_record_file_path.exists():
        with open(time_record_file_path, 'rb') as f:
            file_record = pickle.load(f)
            
    has_info_changed = False
    
    for file in files:
        last_modification_time = os.path.getmtime(file)
        if file in file_record.parsed_file_record:
            new_file_record.parsed_file_record[file] = file_record.parsed_file_record[file]
            new_file_record.mtime[file] = last_modification_time
            if file_record.mtime[file] == last_modification_time:
                continue

        has_info_changed = True
        parse_filename = file
        node = parse_one_file(file, str(parse_dir.parent))
        
        # trivial null Node
        new_file_record.parsed_file_record[file] = Node('')
        new_file_record.mtime[file] = last_modification_time
        if len(node.children) != 0:
            new_file_record.parsed_file_record[file] = node
           
    try:
        os.makedirs(refl_output_dir, exist_ok=True)
    except FileExistsError:
        print(f"Directory '{refl_output_dir}' already exists.", flush=True)
    except PermissionError:
        print(f"Permission denied: Unable to create '{refl_output_dir}'.", flush=True)
    except Exception as e:
        print(f"An error occurred when create dir {refl_output_dir}: {e}", flush=True)

    try:
        os.makedirs(binding_output_dir, exist_ok=True)
    except FileExistsError:
        print(f"Directory '{binding_output_dir}' already exists.", flush=True)
    except PermissionError:
        print(f"Permission denied: Unable to create '{binding_output_dir}'.", flush=True)
    except Exception as e:
        print(f"An error occurred when create dir {binding_output_dir}: {e}", flush=True)

    with open(time_record_file_path, 'wb') as f:
        pickle.dump(new_file_record, f)
        
    refl_impl_data = {'refl_header_files': [], 'func_calls': []}
    script_impl_data = {'binding_header_files': [], 'func_calls': []}
    if has_info_changed:
        # refl code generate
        for path, node in new_file_record.parsed_file_record.items():
            if len(node.children) == 0:
                continue

            func_name, refl_code, binding_code = node_code_generate(str(path), node)
            final_filename = func_name + '.hpp'
            refl_impl_data['refl_header_files'].append({'refl_header_file': final_filename})
            refl_impl_data['func_calls'].append({'func_call': f'register_{func_name}_ReflInfo'})
            print(f'reflection generate code to {final_filename}', flush=True)
            save_generated_code(refl_output_dir / final_filename, refl_code)

            final_filename = func_name + '.hpp'
            script_impl_data['binding_header_files'].append({'binding_header_file': final_filename})
            script_impl_data['func_calls'].append({'func_call': f'register_{func_name}_QJSBinding'})
            print(f'script binding generate code to {final_filename}', flush=True)
            save_generated_code(binding_output_dir / final_filename, binding_code)

    with open(refl_header_filename, 'w+', encoding='utf-8') as f:
        f.write(chevron.render(g_refl_header_mustache, {}))

    with open(refl_impl_filename, 'w+', encoding='utf-8') as f:
        f.write(chevron.render(g_refl_impl_mustache, refl_impl_data))

    with open(binding_header_filename, 'w+', encoding='utf-8') as f:
        f.write(chevron.render(g_script_binding_header_mustache, {}))

    with open(binding_impl_filename, 'w+', encoding='utf-8') as f:
        f.write(chevron.render(g_script_binding_impl_mustache, script_impl_data))