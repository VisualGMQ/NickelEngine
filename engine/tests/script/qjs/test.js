Object.keys(test_module).forEach(key => {
    console.log(`Key: ${key}, Value: ${test_module[key]}`);
});

console.log(test_module.int_elem)
console.log(test_module.uint_elem)
console.log(test_module.char_elem)
console.log(test_module.uchar_elem)
console.log(test_module.long_elem)
console.log(test_module.ulong_elem)
console.log(test_module.float_elem)
console.log(test_module.double_elem)
console.log(test_module.bool_elem)
console.log(test_module.string_view)
console.log(test_module.str_literal)
console.log(test_module.string_elem)

let person = new test_module.Person("John")
console.log(person.const_value)
test_module.Person.SayHello()
console.log(person.age)
person.Introduce()
person.age = 123
console.log(person.age)
console.log(person.height)
console.log(person.name)
person.name = "VisualGMQ"
console.log(person.name)
console.log(test_module.Person.static_elem)

function test_const_var(person) {
    console.log("in test const var")
    console.log(person.age)
    person.age = 33
    console.log(person.age)
}