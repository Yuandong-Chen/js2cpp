var Person = function(n) {
	this.name = n;
};
var name = 'John';
var john = new Person(name);
var a = {};
a.__proto__ = john;
console.log(a.name);
