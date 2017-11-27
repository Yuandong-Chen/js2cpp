var a = function(j) {
	this.name = j;
	this.obj = ('o' + j);
};

var t = 'i';
var b = new a(t);
console.log(b);

var i = 1;
while(i < 5)
{
	console.log(i);
	i = i + 1;
};


