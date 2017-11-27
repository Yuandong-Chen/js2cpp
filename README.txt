semantics:

high order function make a new class _#: public Object {}, it's dynamic binding,
the same meaning of assign shared_ptr<Object> to the right, so:
var a ::= shared_ptr<Object> a;
Node kind:
	root,
	exprlist,
	math_e,
	assign_e,
	bool_e,
	callchain,
	lvalue_l,
	rvalue_l,
	decl_e,
	bbrk,
	ccont,
	rtrn_e,
	while_e,
	if_e,
	for_e,
	minus,
	plus,
	times,
	divide,
	double_c,
	string_c,
	variable_c,
	bool_c,
	and_b,
	or_b,
	not_b,
	islarger_b,
	issmaller_b,
	isequal_b,
	isinequal_b,
	commalist_l,
	colonlist_l,
	new_e,
	function_l,
	this_l,
	attrchain,
	brcschain,
	api_c,
	undefined_l,
	pipechain,
	list_l,
	disc_l,
	nil_l,
	colonlistfirst_l,
	colonlistlast_l,
	colonpair_l

Lexer Rules:
// program ::= expressionlist EOI //the root should be shared_ptr type


// expressionlist ::= expression SEMI expressionlist
// 				| expression SEMI
// 				| SEMI

// expression ::= math_expression
// 			| assign_expression
// 			| bool_expression
// 			| call_chain
// 			| lvalue
// 			| decl_expression
// 			| BREAK
// 			| CONTINUE
// 			| return_expression
// 			| while_expression
// 			| if_expression
// 			| for_expression
// 			| LP expression RP
// 			| nullable


// assign_expression ::= lvalue EQUAL rvalue
// 					| lvalue EQUAL lvalue

// math_expression ::= math_expression MINUS rvalue
// 				| math_expression PLUS rvalue
// 				| math_expression TIMES rvalue 
// 				| math_expression DIVIDE rvalue
// 				| math_expression MINUS lvalue
// 				| math_expression PLUS lvalue 
// 				| math_expression TIMES lvalue 
// 				| math_expression DIVIDE lvalue 
// 				| LP math_expression RP
// 				| lvalue DOUBLE STRING

// bool_expression ::= TFB
// 				| NOT bool_expression 
// 				| bool_expression AND rvalue
// 				| bool_expression OR rvalue 
// 				| math_expression ISLARGER rvalue
// 				| math_expression ISSMALLER rvalue
// 				| bool_expression AND lvalue
// 				| bool_expression OR lvalue 
// 				| math_expression ISLARGER lvalue
// 				| math_expression ISSMALLER lvalue
// 				| lvalue ISINEQUAL rvalue
// 				| lvalue ISINEQUAL lvalue
// 				| lvalue ISEQUAL rvalue
// 				| lvalue ISEQUAL lvalue
// 				| LP bool_expression RP
// 				| lvalue

// decl_expression ::= VAR VARIABLE

// new_expression ::= NEW lvalue LP commalist RP
// 					| NEW lvalue LP RP

// return_expression ::= RETURN rvalue
//					| RETURN

// while_expression ::= WHILE LP bool_expression RP LBB expressionlist RBB

// if_expression ::= IF LP bool_expression RP LBB expressionlist RBB

// for_expression ::= FOR LP assign_expression SEMI bool_expression SEMI assign_expression RP LBB expressionlist RBB

// function ::= FUNCTION LP commalist RP LBB expressionlist RBB
//			| FUNCTION LP RP LBB expressionlist RBB


// lvalue ::= VARIABLE
// 		| THIS
// 		| attribute_chain
// 		| bracket_chain
// 		| decl_expression

// rvalue ::= API
// 		| DOUBLE
// 		| STRING
// 		| UNDEFINED
// 		| math_expression
// 		| bool_expression
// 		| call_chain
// 		| pipe_chain
// 		| list
// 		| disc
// 		| new_expression
// 		| function

// attribute_chain ::= lvalue DOT VARIABLE

// pipe_chain ::= rvalue DOT VARIABLE

// bracket_chain ::= lvalue LB rvalue RB

// call_chain ::= rvalue LP commalist RP
//			| rvalue LP RP
//		 	| lvalue LP commalist RP
//			| lvalue LP RP


// commalist ::= commalist COMMA rvalue
// 			| commalist COMMA lvalue
// 			| lvalue 
// 			| rvalue

// list ::= LB commalist RB
//		| NIL

// disc ::= LBB colonlist RBB
//		| LBB RBB

// colonlist ::= colonlist COMMA VARIABLE COLON lvalue
//			| colonlist COMMA VARIABLE COLON rvalue
//			| VARIABLE COLON lvalue
//			| VARIABLE COLON rvalue

// transferred to following rules:
// colonlist ::= colonlistfirst colonlistlast

// colonlistfirst ::= colonlist COMMA VARIABLE
//					| VARIABLE

// colonlistlast ::= COLON lvalue
//				 | COLON rvalue


Cpp Core Code:
	class Object
	{
	public:								// double, string, bool
		std::string tag = "undefined"; // undefined, disc(print out as {e: e1,...}), list(print out as Array), Function
		bool isFunction = false; // only function can call apply and has scope, __scope__ attribute
		int functionID = -1;
		std::map<std::string, std::shared_ptr<Object>> scope;
		std::pair<std::shared_ptr<Object>, Object *> prototype = {nullptr, this};
		std::map<std::string, std::shared_ptr<Object>> member; // used for a.b, a.b() 
		std::deque<std::shared_ptr<Object>> list;    //we assume var a = function() {}; === function a() {};
		Object *__scope__ = nullptr;
		std::pair<std::shared_ptr<Object>, Object *> __proto__;
		boost::any value; // used for double and string

		Object() = default;
		~Object() = default;

		virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr /* add more params */) {
			// do some work

			return nullptr;
		}

		// no need to forward params
		virtual std::shared_ptr<Object> newObj(/* add more params */) {  // copy member and set __proto__ to this prototype
			std::shared_ptr<Object> ret = std::make_shared<Object>();
			ret->tag = "disc";
			if(this->prototype.first == nullptr) {
				this->prototype.first = std::make_shared<Object>();
				this->prototype.first->tag = "disc";
			}
			ret->__proto__ = this->prototype;
			this->apply(ret /* same to apply */);
			return ret;
		}

		template<typename T>
		std::shared_ptr<T> newFunc(int funcID) {
			std::shared_ptr<T> ret = std::make_shared<T>();
			ret->tag = "Function";
			ret->isFunction = true;
			ret->functionID = funcID;
			ret->__scope__ = this;
			return ret;
		}

	};

	std::shared_ptr<Object>& brackets(std::shared_ptr<Object> o, std::string name) {
		if(o == nullptr || o->tag == "undefined") {
			std::cerr<<"RUNTIME ERROR: cannot modify undefined"<<std::endl;
			exit(1);
		}
		if(o->tag != "disc") {
			std::cerr<<"RUNTIME ERROR: only disc has member"<<std::endl;
			exit(1);
		}
		Object *tryfind = o.get();
		if(name != "prototype" && name != "__proto__") {
			while(tryfind != nullptr) {
				if(tryfind->member.find(name) != tryfind->member.end()) {
					return tryfind->member.at(name);
				}
				else
				{
					try {
						if(tryfind->__proto__.first != nullptr) {
							if(tryfind->__proto__.first->member.find(name) != tryfind->__proto__.first->member.end()) {
								return tryfind->__proto__.first->member.at(name);
							}
						}
						else if(tryfind->__proto__.second != nullptr) {
							tryfind = tryfind->__proto__.second->prototype.first.get();
							continue;
						}
						break;
					}
					catch(...)
					{
						break;
					}
				}
			}
			
			o->member[name] = std::make_shared<Object>();
			return o->member.at(name);
		}
		else if(name == "prototype") {
			if(o->prototype.first == nullptr) {
				o->prototype.first = std::make_shared<Object>();
				o->prototype.first->tag = "disc";
			}
			return o->prototype.first;
		}
		
		o->__proto__.second = nullptr;
		return o->__proto__.first;
	}

	std::shared_ptr<Object>& brackets(std::shared_ptr<Object> o, double i) {
		if(o->tag != "list") {
			std::cerr<<"RUNTIME ERROR: only list has index"<<std::endl;
			exit(1);
		}
		try {
			return o->list.at(i);
		}
		catch(...)
		{
			std::cerr<<"RUNTIME ERROR: out of range"<<std::endl;
			exit(1);
		}
	}

	std::shared_ptr<Object> listwith() {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "list";
		return ret;
	}

	template<typename T, typename ...Args>
	std::shared_ptr<Object> listwith(T t, Args ...args) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "list";
		std::shared_ptr<Object> to;
		objBind(to, t);
		ret = listwith(args...);
		ret->list.push_front(to);
		return ret;
	}

	template<typename ...Args>
	std::shared_ptr<Object> listwith(std::shared_ptr<Object> t, Args ...args) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "list";
		ret = listwith(args...);
		ret->list.push_front(t);
		return ret;
	}

	template<>
	std::shared_ptr<Object> listwith(std::shared_ptr<Object> t) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "list";
		ret->list.push_front(t);
		return ret;
	}

	template<typename T>
	std::shared_ptr<Object> listwith(T t) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "list";
		std::shared_ptr<Object> to;
		objBind(to, t);
		ret->list.push_front(to);
		return ret;
	}


	// !!!!!!! ALERT: we have to renew scope member of this after every bind and alloc clause !!!!!!!
	// and this renewed clause totally auto-generated by compiler
	static void objBind(std::shared_ptr<Object>& o, double i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "double";
		ret->value = i;
		o = ret;
	}

	static void objBind(std::shared_ptr<Object>& o, std::string i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "string";
		ret->value = i;
		o = ret;
	}

	static void objBind(std::shared_ptr<Object>& o, bool i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "bool";
		ret->value = i;
		o = ret;
	}

	static void objBind(std::shared_ptr<Object>& o, const std::shared_ptr<Object>& n) {
		o = n;
	}

	static void renewScope(Object *func, std::shared_ptr<Object> var, std::string name, bool isAlloc = true, int level = 0) {
		if(isAlloc) {
			func->scope[name] = var;
			return;
		}
		int i = 0;
		Object *trace = func;
		while(i < level) {
			trace = trace->__scope__;
			if(trace == nullptr) {
				std::cerr<<"RUNTIME ERROR: "<<name<<" in no scope, cannot be renewed "<<std::endl;
				return;
			}
		}
		trace->scope[name] = var;
	}

	static std::shared_ptr<Object>& objFetch(Object *func, std::string name, int level = 0) {
		int i = 0;
		Object *trace = func;
		while(i < level) {
			trace = trace->__scope__;
			if(trace == nullptr) {
				std::cerr<<"RUNTIME ERROR: "<<name<<" in no scope, cannot be fetched "<<std::endl;
				exit(1);
			}
		}
		return trace->scope[name];
	}

	static std::shared_ptr<Object> objAdd(std::shared_ptr<Object> o, double i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = boost::any_cast<double>(o->value) + i;
		return ret;
	}

	static std::shared_ptr<Object> objAdd(std::shared_ptr<Object> o, std::string s) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag != "string") {
			std::cerr<<"RUNTIME ERROR: plus operation with no string type"<<std::endl;
			return nullptr;
		}
		ret->value = boost::any_cast<std::string>(o->value) + s;
		return ret;
	}

	static std::shared_ptr<Object> objAdd(std::shared_ptr<Object> o, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag == r->tag && o->tag == "string") {
			ret->value = boost::any_cast<std::string>(o->value) + boost::any_cast<std::string>(r->value);
			return ret;
		}
		else if(o->tag == r->tag && o->tag == "double") {
			ret->value = boost::any_cast<double>(o->value) + boost::any_cast<double>(r->value);
			return ret;
		}
		std::cerr<<"RUNTIME ERROR: plus operation with no illegal type"<<std::endl;
		return nullptr;
	}

	static std::shared_ptr<Object> objAdd(double i, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = r->tag;
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = boost::any_cast<double>(r->value) + i;
		return ret;
	}

	static std::shared_ptr<Object> objAdd(std::string s, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = r->tag;
		if(r->tag != "string") {
			std::cerr<<"RUNTIME ERROR: plus operation with no string type"<<std::endl;
			return nullptr;
		}
		ret->value = boost::any_cast<std::string>(r->value) + s;
		return ret;
	}

	static std::shared_ptr<Object> objAdd(double i, double r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->value = r + i;
		return ret;
	}

	static std::shared_ptr<Object> objSub(std::shared_ptr<Object> o, double i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = boost::any_cast<double>(o->value) - i;
		return ret;
	}

	static std::shared_ptr<Object> objSub(std::shared_ptr<Object> o, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag == r->tag && o->tag == "double") {
			ret->value = boost::any_cast<double>(o->value) - boost::any_cast<double>(r->value);
			return ret;
		}
		std::cerr<<"RUNTIME ERROR: plus operation with no illegal type"<<std::endl;
		return nullptr;
	}

	static std::shared_ptr<Object> objSub(double i, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = r->tag;
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = i - boost::any_cast<double>(r->value);
		return ret;
	}

	static std::shared_ptr<Object> objSub(double i, double r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->value = i - r;
		return ret;
	}

	static std::shared_ptr<Object> objMul(std::shared_ptr<Object> o, double i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = i * boost::any_cast<double>(o->value);
		return ret;
	}

	static std::shared_ptr<Object> objMul(std::shared_ptr<Object> o, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag == r->tag && o->tag == "double") {
			ret->value = boost::any_cast<double>(r->value) * boost::any_cast<double>(o->value);
			return ret;
		}
		std::cerr<<"RUNTIME ERROR: plus operation with no illegal type"<<std::endl;
		return nullptr;
	}

	static std::shared_ptr<Object> objMul(double i, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = r->tag;
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = i * boost::any_cast<double>(r->value);
		return ret;
	}

	static std::shared_ptr<Object> objMul(double i, double r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->value = i * r;
		return ret;
	}


	static std::shared_ptr<Object> objDiv(std::shared_ptr<Object> o, double i) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(o->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = i / boost::any_cast<double>(o->value);
		return ret;
	}

	static std::shared_ptr<Object> objDiv(std::shared_ptr<Object> o, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = o->tag;
		if(r->tag == o->tag && o->tag == "double") {
			ret->value = boost::any_cast<double>(o->value) / boost::any_cast<double>(r->value);
			return ret;
		}
		std::cerr<<"RUNTIME ERROR: plus operation with no illegal type"<<std::endl;
		return nullptr;
	}

	static std::shared_ptr<Object> objDiv(double i, std::shared_ptr<Object> r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = r->tag;
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: math operation with no double type"<<std::endl;
			return nullptr;
		}
		ret->value = i / boost::any_cast<double>(r->value);
		return ret;
	}

	static std::shared_ptr<Object> objDiv(double i, double r) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->value = i / r;
		return ret;
	}

	static bool objIsLarger(std::shared_ptr<Object> r, double d) {
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<double>(r->value) > d;
	}

	static bool objIsLarger(double d, std::shared_ptr<Object> r) {
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return d > boost::any_cast<double>(r->value);
	}

	static bool objIsLarger(std::shared_ptr<Object> l, std::shared_ptr<Object> r) {
		if(r->tag != "double" || l->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<double>(l->value) > boost::any_cast<double>(r->value);
	}

	static bool objIsLarger(double r, double d) {
		return r > d;
	}

	/////
	static bool objAnd(std::shared_ptr<Object> r, bool d) {
		if(r->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<bool>(r->value) && d;
	}

	static bool objAnd(bool d, std::shared_ptr<Object> r) {
		if(r->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return d && boost::any_cast<bool>(r->value);
	}

	static bool objAnd(std::shared_ptr<Object> l, std::shared_ptr<Object> r) {
		if(r->tag != "bool" || l->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<bool>(l->value) && boost::any_cast<bool>(r->value);
	}

	static bool objAnd(bool r, bool d) {
		return r && d;
	}

	/////

	static bool objOr(std::shared_ptr<Object> r, bool d) {
		if(r->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<bool>(r->value) || d;
	}

	static bool objOr(bool d, std::shared_ptr<Object> r) {
		if(r->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return d || boost::any_cast<bool>(r->value);
	}

	static bool objOr(std::shared_ptr<Object> l, std::shared_ptr<Object> r) {
		if(r->tag != "bool" || l->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<bool>(l->value) || boost::any_cast<bool>(r->value);
	}

	static bool objOr(bool r, bool d) {
		return r || d;
	}

	/////

	static bool objNot(bool r) {
		return !r;
	}

	static bool objNot(std::shared_ptr<Object> r) {
		if(r->tag != "bool") {
			std::cerr<<"RUNTIME ERROR: bool operation with no double type"<<std::endl;
			return nullptr;
		}
		return !boost::any_cast<bool>(r->value);
	}

	/////
	static bool objIsSmaller(std::shared_ptr<Object> r, double d) {
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<double>(r->value) < d;
	}

	static bool objIsSmaller(double d, std::shared_ptr<Object> r) {
		if(r->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return d < boost::any_cast<double>(r->value);
	}

	static bool objIsSmaller(std::shared_ptr<Object> l, std::shared_ptr<Object> r) {
		if(r->tag != "double" || l->tag != "double") {
			std::cerr<<"RUNTIME ERROR: cmp operation with no double type"<<std::endl;
			return nullptr;
		}
		return boost::any_cast<double>(l->value) < boost::any_cast<double>(r->value);
	}

	static bool objIsSmaller(double r, double d) {
		return r < d;
	}

	static bool objIsEqual(std::shared_ptr<Object> r, bool b) {
		if(r->tag == "bool") {
			return b == boost::any_cast<bool>(r->value);
		}
		return false;
	}

	static bool objIsEqual(std::shared_ptr<Object> r, double d) {
		if(r->tag == "double") {
			return d == boost::any_cast<double>(r->value);
		}
		return false;
	}

	static bool objIsEqual(std::shared_ptr<Object> r, std::string s) {
		if(r->tag == "string") {
			return s == boost::any_cast<std::string>(r->value);
		}
		return false;
	}

	static bool objIsEqual(bool b, std::shared_ptr<Object> r) {
		if(r->tag == "bool") {
			return b == boost::any_cast<bool>(r->value);
		}
		return false;
	}

	static bool objIsEqual(double d, std::shared_ptr<Object> r) {
		if(r->tag == "double") {
			return d == boost::any_cast<double>(r->value);
		}
		return false;
	}

	static bool objIsEqual(std::string s, std::shared_ptr<Object> r) {
		if(r->tag == "string") {
			return s == boost::any_cast<std::string>(r->value);
		}
		return false;
	}

	static bool objIsEqual(std::shared_ptr<Object> l, std::shared_ptr<Object> r) {
		if(l->tag != r->tag) {
			return false;
		}

		if(l->tag == "undefined") {
			return true;
		}
		else if(l->tag == "bool") {
			return boost::any_cast<bool>(l->value) == boost::any_cast<bool>(r->value);
		}
		else if(l->tag == "double") {
			return boost::any_cast<double>(l->value) == boost::any_cast<double>(r->value);
		}
		else if(l->tag == "string") {
			return boost::any_cast<std::string>(l->value) == boost::any_cast<std::string>(r->value);
		}
		else if(l->tag == "Function") {
			return l->functionID == r->functionID;
		}
		else if(l->tag == "list") {
			if(l->list.size() != r->list.size()) {
				return false;
			}
			bool isequal = true;
			for (int i = 0; i < l->list.size(); ++i)
			{
				isequal = objIsEqual(l->list[i], r->list[i]);
				if(isequal == false) {
					return false;
				}
			}
			return true;
		}
		else if(l->tag == "disc") {
			if(l->__proto__ != r->__proto__) {
				return false;
			}
			else if(l->member.size() != r->member.size()) {
				return false;
			}
			bool isequal = true;
			for (auto const& x: l->member)
			{
				try {
					return objIsEqual(x.second, r->member.at(x.first));
				}
				catch(...)
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

	template<typename L, typename R>
	static bool objIsEqual(L l, R r) {
		return (l == r); // no need to forward the params
	}

	template<typename L, typename R>
	static bool objIsInEqual(L l, R r) {
		return !(objIsEqual(l, r)); // no need to forward the params
	}

	void objPrint(std::string o, bool inLine = true) {
		std::cout<<o;
		if(inLine) {
			std::cout<<std::endl;
		}
	}

	void objPrint(double o, bool inLine = true) {
		std::cout<<o;
		if(inLine) {
			std::cout<<std::endl;
		}
	}

	void objPrint(bool o, bool inLine = true) {
		std::cout<<std::boolalpha<<o;
		if(inLine) {
			std::cout<<std::endl;
		}
	}

	void objPrint(std::shared_ptr<Object> o, bool inLine = true) {
		if(o->tag == "undefined") {
			std::cout<<" undefined ";
		}
		else if(o->tag == "disc") {
			std::cout<<" { ";
			for (auto const& x: o->member)
			{
				std::cout<<x.first<<":";
				objPrint(x.second, false);
				std::cout<<"  ";
			}
			std::cout<<" } ";
		}
		else if(o->tag == "list") {
			std::cout<<" [ ";
			for (auto const& x: o->list)
			{
				objPrint(x, false);
			}
			std::cout<<" ] ";
		}
		else if(o->tag == "Function") {
			std::cout<<" Function ";
		}
		else if(o->tag == "string") {
			std::cout<<boost::any_cast<std::string>(o->value)<<" ";
		}
		else if(o->tag == "double") {
			std::cout<<boost::any_cast<double>(o->value)<<" ";
		}
		else if(o->tag == "bool") {
			std::cout<<std::boolalpha<<boost::any_cast<bool>(o->value)<<" ";
		}
		if(inLine) {
			std::cout<<std::endl;
		}
	}

	class __windows__: public Object
	{
	public:
		virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr /* add more params */) {
			// do some work
			std::shared_ptr<minicodegen::Object> l = std::make_shared<minicodegen::Object>();
			renewScope(this, l, "l");
			objBind(objFetch(this, "l", 0), (double)0);
			renewScope(this, objFetch(this, "l", 0), "l", false, 0);
			while(({
				std::shared_ptr<minicodegen::Object> k = std::make_shared<minicodegen::Object>();
				renewScope(this, k, "k");
				objIsSmaller(objFetch(this, "l", 0), (double)5);
			}))
			{
				objPrint(objFetch(this, "l", 0));
				objBind(objFetch(this, "l", 0), objAdd(objFetch(this, "l", 0), (double)1));
				renewScope(this, objFetch(this, "l", 0), "l", false, 0);
			}

			objBind(objFetch(this, "l", 0), minicodegen::listwith(1.0,2.0,3.0,std::make_shared<minicodegen::Object>()));
			std::shared_ptr<minicodegen::Object> o = std::make_shared<minicodegen::Object>();
			renewScope(this, o, "o");
			o->tag = "disc";

			std::shared_ptr<minicodegen::Object> r = std::make_shared<minicodegen::Object>();
			renewScope(this, r, "r");
			r->tag = "disc";
			
			//objBind(, l);
			objBind(brackets(brackets(objFetch(this, "o", 0), "prototype"), "a"), l);
			objBind(objFetch(this, "r", 0), objFetch(this, "o", 0)->newObj());
			minicodegen::objPrint(minicodegen::objNot(minicodegen::objIsEqual(brackets(objFetch(this, "r", 0), "a"),objFetch(this, "l", 0))));
			objPrint(objFetch(this, "l", 0));
			return nullptr;
		}
	};