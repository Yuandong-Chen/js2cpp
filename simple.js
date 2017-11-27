var bubblesort = function(e) {
	var array = e;
	return function(siz) {
		var size = siz;
		var i = size - 1;
		while(i > 0) {
			var j = 0;
			while(j < i) {
				
				if(array[j] > array[j + 1]) {
					var temp = array[j];
					array[j] = array[j + 1];
					array[j + 1] = temp;
				};
				j = j + 1;
			};
			i = i - 1;
		};
	};
};

var arr = [6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1,6,5,4,3,2,1];
var ss = 6 * 8;
bubblesort(arr)(ss);
console.log(arr);
var str = 'Test: ' + 'Success';
console.log(str);
