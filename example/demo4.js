var Animal = function(n) {
	this.id = 'Hello';
};
var Person = function(s) {
	this.name = s;
};
var amy = {id: 'Hello'};

var john = {name: 'John'};
john.__proto__ = amy;
john.prototype = {};
john.prototype.__proto__ = amy;
var a = {};
a.__proto__ = john.prototype;
console.log(a.id);
