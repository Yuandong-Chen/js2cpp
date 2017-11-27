var a = function (c) {
	var s; var a;
	a = [1,2,3];
	s = function() {
			a[1] = 9;
			consolelog(a);
		};

	
	return {
		s,

		f: function () {
			consolelog(a);
		}
	};
}


a().f();
consolelog(a());
