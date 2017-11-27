JavaScript like language to Cpp:

Dependent on: boost/any c++11 

Limitation: Every clause should be ended with semicolon, and function must have and only have ONE argument and this parameter must be passed in as variable, no else and for flow control in flow control clause. Only control.log is supported as API. No module supported. For mathematic operation, please use brackets to eliminate the ambiguity. Only anonymous function is supported, but if you want to use named function, you can easily bind it to a var type and then call this var name as a named function.

Supported features:
new/this keyword, high order function, prototype chain, closure, IIFE(without parenthese), if-clause, while-clause, recursive-call.

Supported types:
dict(work like object), function, list(no apis support such attributs like pop, shift...), string, double, bool, undefined

Different to js hoist mechanism:
The variable declared in local function will cover the existed same named variable in above function space.
The variable used without declaration will not be hoisted as global variable but as local function space variable, so our iife does not need to add parentheses outside the function definition.

How to use it:
./tnode.sh infile.js
