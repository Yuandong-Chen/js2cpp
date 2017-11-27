var global = 0;

var a = function(e) {
	return {
		add: function(e) {
			global = global + 1;
		}
	};	
};
var dummy = undefined;
var add1 = a(dummy).add;
add1(dummy);
console.log(global);
