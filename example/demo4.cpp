#include <iostream>
#include <functional>
#include <map>
#include <deque>
#include <boost/any.hpp>
class Object
{
public:
	std::string tag = "undefined";
	bool isFunction = false;
	int functionID = -1;
	std::map<std::string, std::shared_ptr<Object>> scope;
	std::pair<std::shared_ptr<Object>, Object *> prototype = {nullptr, this};
	std::map<std::string, std::shared_ptr<Object>> member;
	std::deque<std::shared_ptr<Object>> list;
	Object *__scope__ = nullptr;
	std::pair<std::shared_ptr<Object>, Object *> __proto__;
	boost::any value;

	Object() = default;
	~Object() = default;

	virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr, std::shared_ptr<Object> e = nullptr) {
		return nullptr;
	}

	virtual std::shared_ptr<Object> newObj(std::shared_ptr<Object> e = nullptr) {
		std::shared_ptr<Object> ret = std::make_shared<Object>();
		ret->tag = "disc";
		if(this->prototype.first == nullptr) {
			this->prototype.first = std::make_shared<Object>();
			this->prototype.first->tag = "disc";
		}
		ret->__proto__ = this->prototype;
		this->apply(ret, e);
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
	if(name != "prototype" && name != "proto") {
		while(tryfind != nullptr) {
			try {
				if(tryfind->member.find(name) != tryfind->member.end()) {
					return tryfind->member.at(name);
				}
				else
				{
					tryfind = tryfind->__proto__.first.get();
					continue;
				}
			}
			catch(...)
			{
				break;
			}
		}
		
		o->member[name] = std::make_shared<Object>();
		return o->member.at(name);
	}
	else if(name == "prototype") {
		if(o->prototype.first == nullptr) {
			o->prototype.first = std::make_shared<Object>();
			//o->prototype.first->tag = "disc";
		}
		return o->prototype.first;
	}
	
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

static void renewScope(Object *func, std::shared_ptr<Object> var, std::string name, int level = 0, bool isAlloc = false) {
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
i++;
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
i++;
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
	return (l == r);
}

template<typename L, typename R>
static bool objIsInEqual(L l, R r) {
	return !(objIsEqual(l, r));
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

void objPrint(const std::shared_ptr<Object>& o, bool inLine = true) {
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
std::shared_ptr<Object> apiWrapperPrint(std::shared_ptr<Object> o) {
	objPrint(o);
	return std::make_shared<Object>();
}

std::map<std::string, std::function<std::shared_ptr<Object>(std::shared_ptr<Object>)> > API = {
	{"console.log",apiWrapperPrint}
};
class __IRFUNC1: public Object {
public:
virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr, std::shared_ptr<Object> n = nullptr) {
objBind(brackets(__this__, "id"), (std::string)"Hello");
return std::make_shared<Object>();
}

virtual std::shared_ptr<Object> newObj(std::shared_ptr<Object> n) {
std::shared_ptr<Object> ret = std::make_shared<Object>();
ret->tag = "disc";
if(this->prototype.first == nullptr) {
this->prototype.first = std::make_shared<Object>();
this->prototype.first->tag = "disc";
}
ret->__proto__ = this->prototype;
this->apply(ret, n);
return ret;
}
};

class __IRFUNC2: public Object {
public:
virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr, std::shared_ptr<Object> n = nullptr) {
objBind(brackets(__this__, "name"), n);
return std::make_shared<Object>();
}

virtual std::shared_ptr<Object> newObj(std::shared_ptr<Object> n) {
std::shared_ptr<Object> ret = std::make_shared<Object>();
ret->tag = "disc";
if(this->prototype.first == nullptr) {
this->prototype.first = std::make_shared<Object>();
this->prototype.first->tag = "disc";
}
ret->__proto__ = this->prototype;
this->apply(ret, n);
return ret;
}
};

class __window__: public Object {
public:
virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr) {
std::shared_ptr<Object> Animal = std::make_shared<Object>();
renewScope(this, Animal, "Animal");
objBind(Animal, newFunc<__IRFUNC1>(1));
renewScope(this, Animal, "Animal", false);
std::shared_ptr<Object> Person = std::make_shared<Object>();
renewScope(this, Person, "Person");
objBind(Person, newFunc<__IRFUNC2>(2));
renewScope(this, Person, "Person", false);
std::shared_ptr<Object> amy = std::make_shared<Object>();
renewScope(this, amy, "amy");
std::shared_ptr<Object> __IR_variable1 = std::make_shared<Object>();
__IR_variable1->tag = "disc";
renewScope(this, __IR_variable1, "__IR_variable1");
objBind(brackets(__IR_variable1, "id"), (std::string)"Hello");
objBind(amy, __IR_variable1);
renewScope(this, amy, "amy", false);
std::shared_ptr<Object> john = std::make_shared<Object>();
renewScope(this, john, "john");
std::shared_ptr<Object> __IR_variable2 = std::make_shared<Object>();
__IR_variable2->tag = "disc";
renewScope(this, __IR_variable2, "__IR_variable2");
objBind(brackets(__IR_variable2, "name"), (std::string)"John");
objBind(john, __IR_variable2);
renewScope(this, john, "john", false);
objBind(brackets(john, "proto"), amy);
std::shared_ptr<Object> __IR_variable3 = std::make_shared<Object>();
__IR_variable3->tag = "disc";
renewScope(this, __IR_variable3, "__IR_variable3");
objBind(brackets(john, "prototype"), __IR_variable3);
std::shared_ptr<Object> __IR_variable4 = std::make_shared<Object>();
objBind(__IR_variable4, brackets(john, "prototype"));
renewScope(this, __IR_variable4, "__IR_variable4");
objBind(brackets(__IR_variable4, "proto"), amy);
std::shared_ptr<Object> a = std::make_shared<Object>();
renewScope(this, a, "a");
std::shared_ptr<Object> __IR_variable5 = std::make_shared<Object>();
__IR_variable5->tag = "disc";
renewScope(this, __IR_variable5, "__IR_variable5");
objBind(a, __IR_variable5);
renewScope(this, a, "a", false);
std::shared_ptr<Object> __IR_variable6 = std::make_shared<Object>();
objBind(__IR_variable6, brackets(john, "prototype"));
renewScope(this, __IR_variable6, "__IR_variable6");
objBind(brackets(a, "proto"), __IR_variable6);
std::shared_ptr<Object> __IR_variable7 = std::make_shared<Object>();
objBind(__IR_variable7, brackets(a, "id"));
renewScope(this, __IR_variable7, "__IR_variable7");
API["console.log"](__IR_variable7);
return nullptr;
}
};

int main(int argc, char const *argv[]) {
std::shared_ptr<__window__> window = std::make_shared<__window__>();
window->apply(window);
return 0;
}

