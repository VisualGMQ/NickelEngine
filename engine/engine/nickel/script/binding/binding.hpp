/**
 * @page qjs_script_binding QuickJS Script Binding
 *
 * This is a binding framework for QuickJS
 *
 * NOTE: we use [QuickJS-NG](https://github.com/quickjs-ng/quickjs)
 *
 * [QuickJS C++ Binding
 * Tutorial](https://visualgmq.github.io/books/quickjs-binding-tutorial/index.html)
 *
 * ## Why use QuickJS
 *
 * Most game engine use C# or Lua or both of them as script language, the reason
 * to use QuickJS is:
 *
 * 1. QuickJS is lite and fast
 * 2. JS syntax is better than lua
 * 3. QuickJS is eazy to bind(compare with C#)
 *
 * But due to @ref code_generate_page, I can change script language easily(and
 * C# & Lua all has excellent binding framework), this reduces the burden of
 * switching languages.
 *
 * ## Why we need yet another quickjs binding?
 *
 * [quickjspp](https://github.com/ftk/quickjspp) already done binding thing, but
 * it has some shortcuts:
 *
 * 1. can't compile with newest QuickJS-NG
 * 2. less feature
 *
 * So I decided to write my own binding framework.
 *
 * ## Can I use this framework?
 *
 * Though this framework is binding in NickelEngine, but it is eazy to extract
 * out as a single library, it only relies on log system and some traits(in @ref
 * refl_module)
 *
 * ## Features
 *
 * - Convenient binding interface
 * - Auto convert JS params to C++ params when call C++ function in JS(JS to T*,
 * T& or T)
 * - Auto bind detected params(like function parameter)
 * - Auto bind `T*`, `T&`, `const T*`, `cont T&` when bind `T`
 *
 * ## Shortcuts
 *
 * - not support function overload
 * - not support register object to global JS object
 *
 * ## Usage
 *
 * `QJSRuntime` is the fundamental object:
 *
 * ```cpp
 * QJSRuntime runtime;
 * ```
 *
 * then create your own module:
 *
 * ```cpp
 * QJSModule& module = runtime.GetContext().NewModule
 * ```
 *
 * then you can bind something on it:
 *
 * ### Bind fundamentals
 *
 * fundamental elements:
 * @snippet{lineno} qjs_test.cpp Binding Fundamentals Declare
 *
 * binding:
 * @snippet{lineno} qjs_test.cpp Binding Fundamentals
 *
 * using:
 * @snippet{lineno} qjs_test.cpp Binding Fundamentals Usage
 *
 * ### Bind class & enum
 *
 * class & enum declare:
 * @snippet{lineno} qjs_test.cpp Binding Class Declare
 *
 * binding:
 * @snippet{lineno} qjs_test.cpp Binding Class
 *
 * usage:
 * @snippet{lineno} qjs_test.cpp Binding Class Usage
 */

#pragma once

#include "nickel/script/binding/context.hpp"
#include "nickel/script/binding/module.hpp"
#include "nickel/script/binding/runtime.hpp"