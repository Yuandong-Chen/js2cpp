#pragma once
#include <memory>
#include "IRTree.hpp"
// this document handles the typesystem and its symbol table
// only 5 kinds of Space names: undefined, list, disc(created new or return or assign), func, double, strliteral, bool
// we implement all basic functionality in the following steps:
// 0. decl variable + undefined + assign + console.log + func with 0 params
// 1. double + mathop + console.log
// 2. bool + boolop + console.log
// 2. strliteral + math + operator[] + console.log
// 3. list + operator[] + join + push/pop + delete + concat + sort + reverse + console.log
// 4. func + callchain + assign + list with func version + console.log
// 5. newedfunc + list with newedfunc version + console.log
// 6. if/while/for + break + continue

// in order to translate func or newedfunc into cpp member function:
// 1. params number
// 2. func is actually a callable Space
// 3. we use segment code to disassociate this linkage just like extern keywords in cpp

// var a = function(e) {
// 	console.log(e);
// }

// var b = new a(1);

// IR:
// <func #0 #main 0
// alloc a #0 // for var 
// ...

// bind a, @func#1234 // we construct virtual function in space #0

// alloc b #1 // for var
// bindnew b, a: 1

// call ...: ..., ...,
// >

// <func #1234 #0 1
// ...
// >

namespace miniir {
	class Space
	{
	public:
		//std::string tag = "undefined";
		std::map<std::string, std::shared_ptr<Space>> scope; // used for a, a(), a().b(), a().b().c()...
		//std::map<std::string, std::shared_ptr<Space>> prototype; // used for a.b, a.b()
		std::map<std::string, std::shared_ptr<Space>> member; // used for a.b, a.b() 
		//std::deque<std::shared_ptr<Space>> list;    //we assume var a = function() {}; === function a() {};
		Space *__scope__ = nullptr;
		//Space *__proto__ = nullptr;
		//Space *__this__ = this; // the first params of every function always be a __this__ 
		boost::any value;
		std::shared_ptr<Space> input = nullptr;
		std::vector<std::string> inputlist;
		//std::shared_ptr<Space> output = nullptr;
		bool newBind = false;
		Space() = default;
		~Space() = default;
	};

	class IRGen
	{
	public:
		int spaceid = 0;
		std::string code = "";
		std::deque<int> idstack;
		std::deque<std::shared_ptr<Space>> spacestack;
		std::deque<std::string> irstack;
		std::deque<std::string> irstackfinished;
		int currid = 0;
		int tempid = 0;
		std::string tempname = "__IR_variable"; // weird name
		std::shared_ptr<Space> currspace;
		std::string currir;
		
		IRGen() {
			init();
		}
		~IRGen() = default;
		std::pair<std::string, std::string> generateIR(std::shared_ptr<Node>, bool, bool, std::string);
		std::string getIRCode() {
			code = "";
			for (auto const& x: irstackfinished)
			{
				code += x;
			}
			return code;
		}

		void init() {
			spaceid = 0;
			currir = "";
			currid = 0;
			tempid = 0;
			code = "";
			spacestack.clear();
			irstack.clear();
			irstackfinished.clear();
			idstack.clear();
			std::string ir0 = "class __window__: public Object {\n";
			ir0 += "public:\n";
			ir0 += "virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr) {\n";
			currir = ir0;
			std::shared_ptr<Space> space0 = std::make_shared<Space>();
			//space0->tag = "main";
			currspace = space0;
			spaceid++;
		}
	};

	// void spaceClone(std::shared_ptr<Space> left, std::shared_ptr<Space> right) {
	// 	//left->tag = right->tag;
	// 	left->scope = right->scope; // used for a, a(), a().b(), a().b().c()...
	// 	//left->prototype = right->prototype; // used for a.b, a.b()
	// 	left->member = right->member; // used for a.b, a.b() 
	// 	//left->list = right->list;    //we assume var a = function() {}; === function a() {};
	// 	left->__scope__ = right->__scope__;
	// 	//left->__proto__ = right->__proto__;
	// 	left->value = right->value;
	// 	left->input = right->input;
	// 	//left->output = right->output;
	// 	left->newBind = right->newBind;
	// }

	std::pair<std::string, std::string> IRGen::generateIR(std::shared_ptr<Node> tree, bool isNewSpace = true, bool isRVALUE = false, std::string info = "") {
		std::cerr<<"generate tree node kind: "<<tree->kind<<"----"<<std::endl;
		switch(tree->kind) {
			case root: 
			case exprlist: {
				std::pair<std::string, std::string> ep;
				std::string ret = "";
				for (int i = 0; i < tree->branch.size(); ++i)
				{
					ep = generateIR(tree->branch[i], false, false, "_");
					ret += ep.first;
					//ret += ep.second;
				}

				if(isNewSpace) {
					currir += ret;
					currir += "return nullptr;\n";
					currir += "}\n";
					currir += "};\n\n";
					irstackfinished.push_back(currir);
					return {"", ""};
				}
				
				return {ret, ""};
			}

			// while/if
			// ...
			// do
			// ...
			// end
			case for_e:
			case while_e:
			case if_e: {
				std::string ret = "";
				std::pair<std::string, std::string> ep;
				if(tree->kind == while_e) {
					ret += "while(({\n";
				}
				else if(tree->kind == if_e) {
					ret += "if(({\n";
				}
				else {
					std::cerr<<"IRGEN ERROR: contorl flow sentence unsupported yet "<<tree->data<<std::endl;
					return {"", ""};
				}
				ep = generateIR(tree->branch[0], false);
				ret += ep.first;
				ret += "objIsEqual("+ep.second+", "+"true);\n";
				ret +="})) {\n";
				ep = generateIR(tree->branch[1], false);
				ret += ep.first;
				ret +="};\n";
				return {ret, ""};
			}

			case rtrn_e: {
				if(tree->branch.size() == 0) {
					return {"if(reentrant > 1) {\nscope = recurstack.top();\nrecurstack.pop();\n}\nreentrant--;\nreturn;\n", ""};
				}
				else
				{
					std::string ret = "";
					std::pair<std::string, std::string> ep;
					ep = generateIR(tree->branch[0], false, true);
					ret += ep.first;
					ret += "std::shared_ptr<Object> __IRRTRN = " + ep.second + ";\n";
					ret += "if(reentrant > 1) {\nscope = recurstack.top();\nrecurstack.pop();\n}\nreentrant--;\n";
					ret += "return __IRRTRN;\n";
					return {ret, ""};
				}
			}

			case decl_e: {
				// find whether this value exist in curr space
				try {
					currspace->scope.at(tree->branch[0]->data);
					// it exists
					return {"", ""};
				}
				catch(...)
				{
					std::string ret = "";
					ret += "std::shared_ptr<Object> ";
					ret += tree->branch[0]->data;
					ret += " = std::make_shared<Object>();\n";
					std::shared_ptr<Space> newspace = std::make_shared<Space>();
					ret += "renewScope(this, " + tree->branch[0]->data + ", \"" + tree->branch[0]->data + "\");\n";
					currspace->scope[tree->branch[0]->data] = newspace;
					return {ret, tree->branch[0]->data}; //this is (0)!
				}
			}

			case assign_e: {
				if(tree->branch[1]->kind != new_e) {
					std::pair<std::string, std::string> epl;
					std::pair<std::string, std::string> epr;
					std::string ret = "";

					epl = generateIR(tree->branch[0], false, false, "_");
					epr = generateIR(tree->branch[1], false, true);
					ret += epl.first;
					ret += epr.first;
					ret += "objBind(";
					ret += epl.second; // might add member/scope to currspace
					ret += ", ";
					ret += epr.second; // might call
					ret += ");\n";
					if(tree->branch[0]->kind == variable_c || tree->branch[0]->kind == decl_e) {
						if(epl.second[epl.second.length() - 1] != ')') // its (0)
						{
							ret += "renewScope(this, "+epl.second+", "+"\""+epl.second+"\", "+"false);\n";
						}
						else
						{
							std::size_t findComma = epl.second.rfind(',');
							std::size_t findQuot1 = epl.second.find('"');
							std::size_t findQuot2 = epl.second.rfind('"');
							if(findComma != std::string::npos) {
								std::string level = epl.second.substr(findComma+1, epl.second.length() - findComma - 2);
								std::string vname = epl.second.substr(findQuot1+1, findQuot2 - findQuot1 - 1);
								ret += "renewScope(this, objFetch(this, \"" + vname + "\", " + level + ")" + ", " + "\"" + vname + "\", " + "false, " + level + ");\n";
							}
							else
							{
								std::cerr<<"IRGEN ERROR: illegal left variable name "<<tree->data<<std::endl;
								return {"", ""};
							}
						}

					}
					
					return {ret, ""};
				}
				else
				{
					// todo
					std::pair<std::string, std::string> epl;
					std::pair<std::string, std::string> epr;
					std::string ret = "";

					epl = generateIR(tree->branch[0], false, false, "_");
					epr = generateIR(tree->branch[1], false, true, "_");
					ret += epl.first;
					//ret += epr.first;
					ret += "objBind(";
					ret += epl.second; // might add member/scope to currspace
					ret += ", ";
					ret += epr.first; // might call
					ret[ret.length() - 2] = ')';
					ret[ret.length() - 1] = ';';
					ret += "\n";
					if(tree->branch[0]->kind == variable_c || tree->branch[0]->kind == decl_e) {
						if(epl.second[epl.second.length() - 1] != ')') // its (0)
						{
							ret += "renewScope(this, "+epl.second+", "+"\""+epl.second+"\", "+"false);\n";
						}
						else
						{
							std::size_t findComma = epl.second.rfind(',');
							std::size_t findQuot1 = epl.second.find('"');
							std::size_t findQuot2 = epl.second.rfind('"');
							if(findComma != std::string::npos) {
								std::string level = epl.second.substr(findComma+1, epl.second.length() - findComma - 2);
								std::string vname = epl.second.substr(findQuot1+1, findQuot2 - findQuot1 - 1);
								ret += "renewScope(this, objFetch(this, \""+vname + "\", " + level + ")"+", "+"\""+vname+"\", "+"false, " + level + ");\n";
							}
							else
							{
								std::cerr<<"IRGEN ERROR: illegal left variable name "<<tree->data<<std::endl;
								return {"", ""};
							}
						}
					}
					return {ret, ""};
				}
			}

			case bbrk: {
				return {"break;\n", ""};
			}

			case ccont: {
				return {"continue;\n", ""};
			}

			case api_c: {
				return {"", "API[\""+tree->data+"\"]"};
			}

			case bool_c: {
				return {"", "(bool)"+tree->data};
			}
			case string_c: {
				return {"", "std::string(\""+tree->data.substr(1,tree->data.length()-2)+"\")"};
			}
			case double_c: {
				return {"", "(double)"+tree->data};
			}

			case this_l: {
				return {"", "__this__"};
			}

			case undefined_l: {
				return {"", "std::make_shared<Object>()"};
			}

			case nil_l: {
				return {"", "nil"};
			}

			case pipechain:
			case attrchain: {
				std::pair<std::string, std::string> ep;
				std::string right = "";
				std::string ret = "";
				ep = generateIR(tree->branch[0], false, isRVALUE);
				if(tree->branch[1]->kind != variable_c) {
					std::cerr<<"IRGEN ERROR: attrchain end with no variable "<<tree->data<<std::endl;
					return {"", ""};
				}

				right = tree->branch[1]->data;
				if(info != "") {
					return {ep.first, "brackets("+ep.second + ", \"" + right + "\")"};
				}
				// we alloc temperate variable return this vairable
				std::shared_ptr<Space> newspace = std::make_shared<Space>();
				currspace->scope[tempname + std::to_string(tempid)] = newspace;
				ret += ep.first;
				//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
				ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
				ret += "objBind(" + tempname + std::to_string(tempid) + ", brackets(" + ep.second + ", \"" + right + "\"));\n"; // this is (0)!!
				ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
				return {ret, tempname + std::to_string(tempid++)};// this is (0)!!
			}

			case brcschain: {
				std::pair<std::string, std::string> ep;
				std::pair<std::string, std::string> epinner;
				std::string right = "";
				std::string ret = "";
				ep = generateIR(tree->branch[0], false, isRVALUE);
				// if(tree->branch[1]->kind != variable_c) {
				// 	std::cerr<<"IRGEN ERROR: attrchain end with no variable "<<tree->data<<std::endl;
				// 	return {"", ""};
				// }
				epinner = generateIR(tree->branch[1], false, true);
				//right = tree->branch[1]->data;
				ret += epinner.first;
				if(info != "") {
					return {ep.first + epinner.first, "brackets(" + ep.second + ", " + epinner.second + ")"};
				}
				// we alloc temperate variable return this vairable
				std::shared_ptr<Space> newspace = std::make_shared<Space>();
				currspace->scope[tempname + std::to_string(tempid)] = newspace;
				ret += ep.first;
				//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
				ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
				ret += "objBind(" + tempname + std::to_string(tempid) + ", brackets(" + ep.second + ", " + epinner.second + "));\n";
				ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
				return {ret, tempname + std::to_string(tempid++)}; //this is (0)!!
			}

			case new_e:
			case callchain: {
				std::pair<std::string, std::string> ep;
				std::string ret = "";
				std::string funcS = "";
				std::string paramL = "";
				if(tree->branch.size() == 2) {
					if(tree->branch[1]->kind != commalist_l) {
						std::cerr<<"IRGEN ERROR: params are not type commalist "<<std::endl;
						return {"", ""};
					}
					ep = generateIR(tree->branch[0], false, isRVALUE);
					ret += ep.first;
					funcS = ep.second;
					int i = 0;
					for (; i < tree->branch[1]->branch.size() - 1; ++i)
					{
						ep = generateIR(tree->branch[1]->branch[i], false, isRVALUE);
						ret += ep.first;
						paramL += ep.second + ", ";
					}
					ep = generateIR(tree->branch[1]->branch[i], false, isRVALUE);
					ret += ep.first;
					paramL += ep.second;

					if(info != "") {
						if(tree->kind == callchain) {
							if(tree->branch[0]->kind == api_c) {
								return {ret + funcS + "(" + paramL + ");\n", ""};
							}
							if(funcS.substr(0,8) == "newFunc<") {
								return {ret + funcS + "->apply"+ "(nullptr, " + paramL + ");\n", ""};
							}
							return {ret + funcS + "->apply"+ "(" + funcS +", " + paramL + ");\n", ""};
						}
						else if(tree->kind == new_e) {
							if(tree->branch[0]->kind == api_c) {
								return {ret + funcS + "(" + paramL + ");\n", ""};
							}
							return {ret + funcS + "->newObj"+ "(" + paramL + ");\n", ""};
						}
						else
						{
							if(tree->branch[0]->kind == api_c) {
								return {"", ret + funcS + "(" + paramL + ")"};
							}
							if(funcS.substr(0,8) == "newFunc<") {
								return {"", ret + funcS + "->apply"+ "(nullptr, " + paramL + ")"};
							}
							return {"", ret + funcS + "->apply"+ "(" + funcS +", " + paramL + ")"};
						}
					}
					else
					{
						std::shared_ptr<Space> newspace = std::make_shared<Space>();
						currspace->scope[tempname + std::to_string(tempid)] = newspace;
						//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
						ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
						ret += "objBind(" + tempname + std::to_string(tempid) + ", ";
						if(tree->branch[0]->kind == api_c) {
							ret += funcS + "(" + paramL + "));\n";
						}
						else
						{
							if(funcS.substr(0,8) == "newFunc<") {
								ret += funcS + "->apply"+ "(nullptr, " + paramL + "));\n";
							}
							else
							{
								ret += funcS + "->apply" + "(" + funcS + ", " + paramL + "));\n";
							}
						}
						ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
						return {ret, tempname + std::to_string(tempid++)}; // this is (0);
					}
				}
				else
				{
					ep = generateIR(tree->branch[0], false, isRVALUE);
					ret += ep.first;
					funcS = ep.second;

					if(info != "") {
						if(tree->branch[0]->kind == api_c) {
							return {ret + funcS + "(" + ");\n", ""};
						}
						if(funcS.substr(0,8) == "newFunc<") {
							return {ret + funcS + "->apply();\n", ""};
						}
						return {ret + funcS + "->apply"+ "(" + funcS + ");\n", ""};
					}
					else
					{
						std::shared_ptr<Space> newspace = std::make_shared<Space>();
						currspace->scope[tempname + std::to_string(tempid)] = newspace;
						//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
						ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
						ret += "objBind(" + tempname + std::to_string(tempid) + ", ";
						if(tree->branch[0]->kind == api_c) {
							ret += funcS + "());\n";
						}
						else
						{
							if(funcS.substr(0,8) == "newFunc<") {
								ret += funcS + "->apply());\n";
							}
							else
							{
								ret += funcS + "->apply" + "(" + funcS  + "));\n";
							}	
						}
						ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
						return {ret, tempname + std::to_string(tempid++)}; //this is (0)
					}
				}
			}

			case list_l: {
				std::pair<std::string, std::string> ep;
				std::string ret = "";
				std::string paramL = "";
				if(tree->branch.size() > 0) {
					int i = 0;
					for (; i < tree->branch.size() - 1; ++i)
					{
						ep = generateIR(tree->branch[i], false, true);
						ret += ep.first;
						paramL += ep.second + ", ";
					}
					ep = generateIR(tree->branch[i], false, true);
					ret += ep.first;
					paramL += ep.second;
					std::shared_ptr<Space> newspace = std::make_shared<Space>();
					currspace->scope[tempname + std::to_string(tempid)] = newspace;
					//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
					ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
					ret += "objBind(" + tempname + std::to_string(tempid) + ", " + "listwith " + "(" + paramL + "));\n";
					ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
					return {ret, tempname + std::to_string(tempid++)}; // this is (0)!!!
				}
				else if(tree->branch.size() == 0)
				{
					std::shared_ptr<Space> newspace = std::make_shared<Space>();
					currspace->scope[tempname + std::to_string(tempid)] = newspace;
					//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
					ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
					ret += "objBind(" + tempname + std::to_string(tempid) + ", " + "listwith " + "());\n";
					ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
					return {ret, tempname + std::to_string(tempid++)}; // this is (0)!!!
				}

				std::cerr<<"IRGEN ERROR: fatal "<<tree->data<<std::endl;
				return {"", ""};
			}

			case disc_l: {
				std::pair<std::string, std::string> ep;
				std::string ret = "";
				std::shared_ptr<Space> newspace = std::make_shared<Space>();
				currspace->scope[tempname + std::to_string(tempid)] = newspace;
				//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
				ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
				ret += tempname + std::to_string(tempid) + "->tag = " + "\"disc\";\n";
				//ret += "tag " + tempname + std::to_string(tempid) + ", $disc" + "\n";
				ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
				if(tree->branch.size() == 0)
				{
					return {ret, tempname + std::to_string(tempid++)}; // this is (0);
				}
				else
				{
					int saveid = tempid;
					for (int i = 0; i < tree->branch.size(); ++i)
					{
						ep = generateIR(tree->branch[i]->branch[0], false, true, "_");
						ret += ep.first;
						std::string vname = ep.second;
						ep = generateIR(tree->branch[i]->branch[1], false, true);
						ret += ep.first;
						ret += "objBind(brackets(" + tempname + std::to_string(saveid) + ", \"" + vname + "\"), " + ep.second + ");\n";
					}
					tempid++;
					return {ret, tempname + std::to_string(saveid)}; // this is (0);
				}

				std::cerr<<"IRGEN ERROR: fatal "<<tree->data<<std::endl; 
				return {"", ""};
			}

			case not_b: {
				std::pair<std::string, std::string> ep;
				std::string ret = "";
				ep = generateIR(tree->branch[0], false, true);
				ret += ep.first;
				std::shared_ptr<Space> newspace = std::make_shared<Space>();
				currspace->scope[tempname + std::to_string(tempid)] = newspace;
				//ret += "alloc " + tempname + std::to_string(tempid) + ";\n";
				ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
				ret += "objBind(" + tempname + std::to_string(tempid) + ", "; // this is (0)!!!
				ret += "objNot(";
				ret += ep.second + "));\n";
				ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
				return {ret, tempname + std::to_string(tempid++)}; // this is (0)!!
			}

			case and_b:
			case or_b:
			case islarger_b:
			case issmaller_b:
			case isequal_b:
			case isinequal_b:
			case plus:
			case minus:
			case times:
			case divide: {
				std::pair<std::string, std::string> epl;
				std::pair<std::string, std::string> epr;
				std::string ret = "";

				epl = generateIR(tree->branch[0], false, true);
				epr = generateIR(tree->branch[1], false, true);
				ret += epl.first;
				ret += epr.first;
				// we do sth like callchain here
				std::shared_ptr<Space> newspace = std::make_shared<Space>();
				currspace->scope[tempname + std::to_string(tempid)] = newspace;
				ret += "std::shared_ptr<Object> " + tempname + std::to_string(tempid) + " = std::make_shared<Object>();\n";
				ret += "objBind(" + tempname + std::to_string(tempid) + ", "; // this is (0)!!!

				if(tree->kind == plus) {
					ret += "objAdd(";
				}
				else if(tree->kind == minus) {
					ret += "objSub(";
				}
				else if(tree->kind == times) {
					ret += "objMul(";
				} 
				else if(tree->kind == divide) {
					ret += "objDiv(";
				}
				else if(tree->kind == and_b) {
					ret += "objAnd(";
				}
				else if(tree->kind == or_b) {
					ret += "objOr(";
				}
				else if(tree->kind == islarger_b) {
					ret += "objIsLarger(";
				}
				else if(tree->kind == issmaller_b) {
					ret += "objIsSmaller(";
				}
				else if(tree->kind == isequal_b) {
					ret += "objIsEqual(";
				}
				else if(tree->kind == isinequal_b) {
					ret += "objIsInequal(";
				}

				ret += epl.second + ", ";
				ret += epr.second + "));\n";
				ret += "renewScope(this, "+tempname + std::to_string(tempid)+", \""+tempname + std::to_string(tempid)+"\");\n";
				return {ret, tempname + std::to_string(tempid++)}; // this is (0)!!!
			}

			case variable_c: {
				bool findScope = false;
				int backLevel = 0;
				//check param space first
				if(currspace->input != nullptr) {
					try {
						currspace->input->member.at(tree->data);
						return {"", tree->data};//this is (0)!!!
					}
					catch(...)
					{
						//std::cerr<<"NOT IN PARAMS"<<std::endl;
					}
				}
				
				Space *finded = currspace.get();
				while(findScope == false && finded != nullptr) {
					try {
						finded->scope.at(tree->data);
						findScope = true;
						break;
					}
					catch(...)
					{
						finded = finded->__scope__;
						backLevel++;
					}
				}
				if(findScope == false) {
					// create new variable in this scope
					if(isRVALUE == false) {
						std::shared_ptr<Space> newspace = std::make_shared<Space>();
						currspace->scope[tree->data] = newspace;
						return {"std::shared_ptr<Object> " + tree->data + " = std::make_shared<Object>();\nrenewScope(this, "+tree->data+", \""+ tree->data+"\");\n",
						tree->data}; //this is (0)!!!
					}

					if(info != "") {
						return {"", tree->data};//this is (0)!!!
					}

					std::cerr<<"IRGEN ERROR: variable in no scope: "<<tree->data<<std::endl;
					return {tree->data + "(?)", ""};
				}

				// if(backLevel == 0)
				// {
				// 	return {"", tree->data}; //this is (0)!!!
				// }
				return {"", "objFetch(this, \""+tree->data + "\", " + std::to_string(backLevel) + ")"};
			}

			case function_l: {
				// push currir currid currspace first like assembly code
				// note carefull about isNewSpace bool tag
				idstack.push_back(currid);
				irstack.push_back(currir);
				spacestack.push_back(currspace);
				currid = spaceid++;
				int paramNum = 0;
				std::string ret = "";
				if(tree->branch[0]->kind == commalist_l) {
					paramNum = tree->branch[0]->branch.size();
				}
				//currir = "<func " + std::to_string(currid) +" "+ std::to_string(idstack.back()) +" "+ std::to_string(paramNum) + "\n";
				currir = "class __IRFUNC" + std::to_string(currid) +": public Object {\n"
				+ "public:\n"
				+ "virtual std::shared_ptr<Object> apply(std::shared_ptr<Object> __this__ = nullptr";
				std::shared_ptr<Space> newSpace = std::make_shared<Space>();
				//newSpace->tag = "func";
				currspace = newSpace;
				currspace->__scope__ = spacestack.back().get();
				if(paramNum == 0)
				{
					ret += "){\n";
					ret += "reentrant++;\nif(reentrant > 1) {\nrecurstack.push(scope);\nscope.clear();\n}\n";
					ret += generateIR(tree->branch[0], false).first;
					ret += "if(reentrant > 1) {\nscope = recurstack.top();\nrecurstack.pop();\n}\nreentrant--;\n";
					ret += "return std::make_shared<Object>();\n";
					ret += "}\n\n";
				}
				else
				{
					// set input
					currspace->input = std::make_shared<Space>();
					for (auto const& x: tree->branch[0]->branch)
					{
						if(x->kind == variable_c) {
							currspace->input->member[x->data] = std::make_shared<Space>();
							currspace->inputlist.push_back(x->data);
							ret += ", std::shared_ptr<Object> "+x->data+" = nullptr";
						}
						else
						{
							std::cerr<<"IRGEN ERROR: function params are not pure variable!: "<<std::endl;
						}
					}
					ret += ") {\n";
					ret += "reentrant++;\nif(reentrant > 1) {\nrecurstack.push(scope);\nscope.clear();\n}\n";
					ret += generateIR(tree->branch[1], false).first;
					ret += "if(reentrant > 1) {\nscope = recurstack.top();\nrecurstack.pop();\n}\nreentrant--;\n";
					ret += "return std::make_shared<Object>();\n";
					ret += "}\n\n";
					ret += "virtual std::shared_ptr<Object> newObj(";
					int i = 0;
					for (; i < currspace->inputlist.size() - 1; i++)
					{
						ret += "std::shared_ptr<Object> " + currspace->inputlist[i] + ", ";
					}
					ret += "std::shared_ptr<Object> " + currspace->inputlist[i];
					ret += ") {\n";
					ret += "std::shared_ptr<Object> ret = std::make_shared<Object>();\n";
					ret += "ret->tag = \"disc\";\n";
					ret += "if(this->prototype.first == nullptr) {\n";
					ret += "this->prototype.first = std::make_shared<Object>();\n";
					ret += "this->prototype.first->tag = \"disc\";\n";
					ret += "}\n";
					ret += "ret->__proto__ = this->prototype;\n";
					ret += "this->apply(ret, ";
					i = 0;
					for (; i < currspace->inputlist.size() - 1; i++)
					{
						ret += currspace->inputlist[i] + ", ";
					}
					ret += currspace->inputlist[i];
					ret += ");\n";
					ret += "return ret;\n";
					ret += "}\n";
				}
				currir += ret;
				currir += "};\n\n";
				irstackfinished.push_back(currir);
				int saveid = currid;

				currspace = spacestack.back();
				spacestack.pop_back();
				currir = irstack.back();
				irstack.pop_back();
				currid = idstack.back();
				idstack.pop_back();
				return {"", "newFunc<__IRFUNC" + std::to_string(saveid) + ">(" + std::to_string(saveid) + ")"};
			}
		}
		return {"", ""};
	}
}

// int main(int argc, char const *argv[])
// {
// 	miniir::IRTree irtree;
// 	miniparser::AST::printASTTree(irtree.getIRTree());
// 	miniir::IRGen irgen;
// 	irgen.generateIR(irtree.getIRTree());
// 	std::cerr<<irgen.getIRCode()<<std::endl;
// 	return 0;
// }



