#pragma once
#include "LRParser.hpp"

class Node {
public:
	int kind;
	std::string data;
	std::deque<std::shared_ptr<Node>> branch;
	Node() = default;
	~Node() = default;
};

enum kind {
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
};

// program ::= expressionlist EOI //the root should be shared_ptr type

void function0_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function0_0: "<<std::endl;
		miniparser::attributeForParentNode = valueStack.at(valueStack.size() - 2); // element should be kind root
		boost::any_cast<std::shared_ptr<Node>>(miniparser::attributeForParentNode)->kind = root;
}

// expressionlist ::= expression SEMI expressionlist
// 				| expression SEMI
// 				| SEMI

void function1_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function1_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = exprlist;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(right->kind != exprlist) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		
		element->branch = right->branch;
		element->branch.push_front(left);
		miniparser::attributeForParentNode = element;
}

void function1_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function1_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = exprlist;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		element->branch.push_back(node);
		miniparser::attributeForParentNode = element;
}

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

void function2_0(std::deque<boost::any>& valueStack, int kind = math_e) {
		std::cerr<<"function2_0: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != kind) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		miniparser::attributeForParentNode = node;
}

void function2_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function2_1: "<<std::endl;
		miniparser::attributeForParentNode = valueStack.at(valueStack.size() - 2);
}

// assign_expression ::= lvalue EQUAL rvalue
// 					| lvalue EQUAL lvalue

void function3_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function3_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = assign_e;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(left->kind == lvalue_l && (right->kind == lvalue_l || right->kind == rvalue_l)) {
			element->branch.push_back(left);
			element->branch.push_back(right);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

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

void function4_0(std::deque<boost::any>& valueStack, int kind = minus) {
		std::cerr<<"function4_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = kind;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(left->kind == math_e && (right->kind == lvalue_l || right->kind == rvalue_l)) {
			element->branch.push_back(left);
			element->branch.push_back(right);
			std::shared_ptr<Node> bridge = std::make_shared<Node>();
			bridge->kind = math_e;
			bridge->branch.push_back(element);
			miniparser::attributeForParentNode = bridge;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

void function4_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function4_1: "<<std::endl;
		miniparser::attributeForParentNode = valueStack.at(valueStack.size() - 2);
}

void function4_2(std::deque<boost::any>& valueStack, int kind = lvalue_l) {
		std::cerr<<"function4_2: "<<std::endl;
		std::shared_ptr<Node> element;
		if(kind != lvalue_l)
		{
			element = std::make_shared<Node>();
			element->kind = kind;
			element->data = minilex::pyytext[0];
		}
		else
		{
			element = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
			if(element->kind != lvalue_l) {
				std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
				exit(1);
			}
		}

		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = math_e;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

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

void function5_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function5_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = bool_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = bool_e;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function5_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function5_1: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != bool_e) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		std::shared_ptr<Node> element = std::make_shared<Node>();
		bridge->kind = bool_e;
		element->kind = not_b;
		element->branch.push_back(node);
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function5_2(std::deque<boost::any>& valueStack, int kind = and_b) {
		std::cerr<<"function5_2: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if((right->kind != rvalue_l && right->kind != lvalue_l)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		std::shared_ptr<Node> element = std::make_shared<Node>();
		bridge->kind = bool_e;
		element->kind = kind;
		element->branch.push_back(left);
		element->branch.push_back(right);
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function5_3(std::deque<boost::any>& valueStack) {
		std::cerr<<"function5_3: "<<std::endl;
		miniparser::attributeForParentNode = valueStack.at(valueStack.size() - 2);
}

void function5_4(std::deque<boost::any>& valueStack) {
		std::cerr<<"function5_4: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != lvalue_l) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = bool_e;
		bridge->branch.push_back(node);
		miniparser::attributeForParentNode = bridge;
}

// decl_expression ::= VAR VARIABLE

void function6_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function6_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = variable_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = decl_e;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

// new_expression ::= NEW lvalue LP commalist RP
// 					| NEW lvalue LP RP

void function7_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function7_0: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 4));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((right->kind != commalist_l && left->kind != lvalue_l)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = new_e;
		element->branch.push_back(left);
		element->branch.push_back(right);
		miniparser::attributeForParentNode = element;
}

void function7_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function7_1: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		if((left->kind != lvalue_l)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = new_e;
		element->branch.push_back(left);
		miniparser::attributeForParentNode = element;
}

// return_expression ::= RETURN rvalue
//					| RETURN

void function8_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function8_0: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if((left->kind != rvalue_l)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = rtrn_e;
		element->branch.push_back(left);
		miniparser::attributeForParentNode = element;
}

void function8_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function8_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = rtrn_e;
		miniparser::attributeForParentNode = element;
}

// while_expression ::= WHILE LP bool_expression RP LBB expressionlist RBB

void function9_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function9_0: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 5));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((left->kind != bool_e || right->kind != exprlist)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			std::cerr<<"EXPECT LEFT "<<bool_e<<" ACTUAL "<<left->kind<<std::endl;
			std::cerr<<"EXPECT RIGHT "<<exprlist<<" ACTUAL "<<right->kind<<std::endl;
			exit(1);
		}

		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = while_e;
		element->branch.push_back(left);
		element->branch.push_back(right);
		miniparser::attributeForParentNode = element;
}

// if_expression ::= IF LP bool_expression RP LBB expressionlist RBB

void function10_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function10_0: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 5));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((left->kind != bool_e && right->kind != exprlist)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}

		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = if_e;
		element->branch.push_back(left);
		element->branch.push_back(right);
		miniparser::attributeForParentNode = element;
}

// for_expression ::= FOR LP assign_expression SEMI bool_expression SEMI assign_expression RP LBB expressionlist RBB

void function11_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function11_0: "<<std::endl;
		std::shared_ptr<Node> init = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 9));
		std::shared_ptr<Node> cond = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 7));
		std::shared_ptr<Node> advances = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 5));
		std::shared_ptr<Node> exprs = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));

		if((init->kind != assign_e && cond->kind != bool_e && advances->kind != assign_e && exprs->kind != exprlist)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}

		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = for_e;
		element->branch.push_back(init);
		element->branch.push_back(cond);
		element->branch.push_back(advances);
		element->branch.push_back(exprs);
		miniparser::attributeForParentNode = element;
}

// function ::= FUNCTION LP commalist RP LBB expressionlist RBB
//			| FUNCTION LP RP LBB expressionlist RBB


void function12_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function12_0: "<<std::endl;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 5));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((right->kind != exprlist && left->kind != commalist_l)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = function_l;
		element->branch.push_back(left);
		element->branch.push_back(right);
		miniparser::attributeForParentNode = element;
}

void function12_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function12_1: "<<std::endl;
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((right->kind != exprlist)) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = function_l;
		element->branch.push_back(right);
		miniparser::attributeForParentNode = element;
}

// lvalue ::= VARIABLE
// 		| THIS
// 		| attribute_chain
// 		| bracket_chain
// 		| decl_expression

void function13_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function13_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = variable_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = lvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function13_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function13_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = this_l;
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = lvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function13_2(std::deque<boost::any>& valueStack, int kind = attrchain) {
		std::cerr<<"function13_2: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != kind)
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = lvalue_l;
		bridge->branch.push_back(node);
		miniparser::attributeForParentNode = bridge;
}

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

void function14_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function14_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = api_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = rvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function14_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function14_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = double_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = rvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function14_2(std::deque<boost::any>& valueStack) {
		std::cerr<<"function14_2: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = string_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = rvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function14_3(std::deque<boost::any>& valueStack) {
		std::cerr<<"function14_3: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = undefined_l;
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = rvalue_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function14_4(std::deque<boost::any>& valueStack, int kind = math_e) {
		std::cerr<<"function14_4: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != kind)
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			std::cerr<<"EXPECT "<<kind<<" ACTUAL "<<node->kind<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = rvalue_l;
		bridge->branch.push_back(node);
		miniparser::attributeForParentNode = bridge;
}

// attribute_chain ::= lvalue DOT VARIABLE

void function15_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function15_0: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		if(node->kind != lvalue_l) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = variable_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = attrchain;
		bridge->branch.push_back(node);
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

// pipe_chain ::= rvalue DOT VARIABLE

void function16_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function16_0: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		if(node->kind != rvalue_l) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = variable_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = pipechain;
		bridge->branch.push_back(node);
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

// bracket_chain ::= lvalue LB rvalue RB

void function17_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function17_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = brcschain;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 4));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if(left->kind == lvalue_l && right->kind == rvalue_l) {
			element->branch.push_back(left);
			element->branch.push_back(right);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

// call_chain ::= rvalue LP commalist RP
//			| rvalue LP RP
//		 	| lvalue LP commalist RP
//			| lvalue LP RP

void function18_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function18_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = callchain;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 4));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if((left->kind == rvalue_l || left->kind == lvalue_l) && right->kind == commalist_l) {
			element->branch.push_back(left);
			element->branch.push_back(right);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

void function18_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function18_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = callchain;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		if((left->kind == rvalue_l || left->kind == lvalue_l)) {
			element->branch.push_back(left);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

// commalist ::= commalist COMMA rvalue
// 			| commalist COMMA lvalue
// 			| lvalue 
// 			| rvalue

void function19_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function19_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = commalist_l;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if((right->kind == rvalue_l || right->kind == lvalue_l) && left->kind == commalist_l) {
			element->branch = left->branch;
			element->branch.push_back(right);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

void function19_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function19_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = commalist_l;
		std::shared_ptr<Node> right = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if((right->kind == rvalue_l || right->kind == lvalue_l)) {
			element->branch.push_back(right);
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

// list ::= LB commalist RB
//		| NIL

void function20_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function20_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = list_l;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if(left->kind == commalist_l) {
			element->branch = left->branch;
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

void function20_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function20_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = list_l;
		miniparser::attributeForParentNode = element;
		
}

// disc ::= LBB colonlist RBB
//		| LBB RBB

void function21_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function21_0: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = disc_l;
		std::shared_ptr<Node> left = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		if(left->kind == colonlist_l) {
			element->branch = left->branch;
			element->kind = disc_l;
			miniparser::attributeForParentNode = element;
		}
		else
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
}

void function21_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function21_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = disc_l;
		miniparser::attributeForParentNode = element;	
}

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

void function22_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function22_0: "<<std::endl;
		std::shared_ptr<Node> colonfirst = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 2));
		std::shared_ptr<Node> colonlast = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = colonlist_l;
		std::shared_ptr<Node> paire = std::make_shared<Node>();
		paire->kind = colonpair_l;
		if(colonfirst->kind != colonlistfirst_l || colonlast->kind != colonlistlast_l) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		else
		{
			paire->branch.push_back(colonfirst->branch.back());
			paire->branch.push_back(colonlast->branch.back());
			for (int i = 0; i < colonfirst->branch.size() - 1; ++i)
			{
				element->branch.push_back(colonfirst->branch[i]);
			}
			element->branch.push_back(paire);
			miniparser::attributeForParentNode = element;
		}
}

void function23_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function23_0: "<<std::endl;
		std::shared_ptr<Node> colonll= boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 3));
		if(colonll->kind != colonlist_l) {
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		else
		{
			std::shared_ptr<Node> element = std::make_shared<Node>();
			element->kind = colonlistfirst_l;
			std::shared_ptr<Node> vv = std::make_shared<Node>();
			vv->kind = variable_c;
			vv->data = minilex::pyytext[0];

			element->branch = colonll->branch;
			element->branch.push_back(vv);
			miniparser::attributeForParentNode = element;
		}
}

void function23_1(std::deque<boost::any>& valueStack) {
		std::cerr<<"function23_1: "<<std::endl;
		std::shared_ptr<Node> element = std::make_shared<Node>();
		element->kind = variable_c;
		element->data = minilex::pyytext[0];
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = colonlistfirst_l;
		bridge->branch.push_back(element);
		miniparser::attributeForParentNode = bridge;
}

void function24_0(std::deque<boost::any>& valueStack) {
		std::cerr<<"function24_0: "<<std::endl;
		std::shared_ptr<Node> node = boost::any_cast<std::shared_ptr<Node>>(valueStack.at(valueStack.size() - 1));
		if(node->kind != lvalue_l && node->kind != rvalue_l)
		{
			std::cerr<<"AST ERROR at line "<<minilex::yyline<<" when reduce at "<<minilex::pyytext[0]<<std::endl;
			exit(1);
		}
		std::shared_ptr<Node> bridge = std::make_shared<Node>();
		bridge->kind = colonlistlast_l;
		bridge->branch.push_back(node);
		miniparser::attributeForParentNode = bridge;
}

namespace miniparser {

	std::map<int, Symbols> symbolMap = {
		{program, miniparser::Symbols(program, false, {{expressionlist, EOI}})},
		{expressionlist, miniparser::Symbols(expressionlist, false, {{expression, SEMI, expressionlist}, {expression, SEMI}, {SEMI}})},
		{expression, miniparser::Symbols(expression, true, {{math_expression}, 
			{assign_expression}, {bool_expression}, {call_chain}, 
			{lvalue}, {decl_expression}, {BREAK},{CONTINUE}, {return_expression},
			{while_expression},{if_expression},{for_expression},
			{LP, expression, RP}})},
		{assign_expression, miniparser::Symbols(assign_expression, false, {{lvalue, EQUAL, rvalue}, 
			{lvalue, EQUAL, lvalue}})},
		{math_expression, miniparser::Symbols(math_expression, false, 
			{{math_expression, MINUS, rvalue},
			{math_expression, PLUS, rvalue}, 
			{math_expression, TIMES, rvalue}, 
			{math_expression, DIVIDE, rvalue},
			{math_expression, MINUS, lvalue},
			{math_expression, PLUS, lvalue}, 
			{math_expression, TIMES, lvalue}, 
			{math_expression, DIVIDE, lvalue}, 
			{LP, math_expression, RP},
			{lvalue},{DOUBLE}, {STRING}})},
		{bool_expression, miniparser::Symbols(bool_expression, false, {{TFB}, 
			{NOT, bool_expression}, 
			{bool_expression, AND, rvalue}, 
			{bool_expression, OR, rvalue}, 
			{math_expression, ISLARGER, rvalue}, 
			{math_expression, ISSMALLER, rvalue},
			{bool_expression, AND, lvalue}, 
			{bool_expression, OR, lvalue}, 
			{math_expression, ISLARGER, lvalue}, 
			{math_expression, ISSMALLER, lvalue},
			{lvalue, ISINEQUAL, rvalue},
			{lvalue, ISINEQUAL, lvalue},
			{lvalue, ISEQUAL, rvalue},
			{lvalue, ISEQUAL, lvalue},
			{LP, bool_expression, RP},
			{lvalue}})},

		{decl_expression, miniparser::Symbols(decl_expression, false, {{VAR, VARIABLE}})},
		{new_expression, miniparser::Symbols(new_expression, false, {{NEW, lvalue, LP, commalist, RP},
			{NEW, lvalue, LP, RP}})},
		{return_expression, miniparser::Symbols(return_expression, false, {{RETURN, rvalue}, {RETURN}})},
		{while_expression, miniparser::Symbols(while_expression, false, {{WHILE, LP, bool_expression, RP, LBB, expressionlist, RBB}})},
		{if_expression, miniparser::Symbols(if_expression, false, {{IF, LP, bool_expression, RP, LBB, expressionlist, RBB}})},
		{for_expression, miniparser::Symbols(for_expression, false, 
			{{FOR, LP, assign_expression,SEMI,bool_expression,SEMI,assign_expression, RP, LBB, expressionlist, RBB}})},
		{function, miniparser::Symbols(function, false, {{FUNCTION, LP, commalist, RP, LBB, expressionlist, RBB}, 
			{FUNCTION, LP, RP, LBB, expressionlist, RBB}})},

		{lvalue, miniparser::Symbols(lvalue, false, {{VARIABLE}, {THIS}, {attribute_chain},{bracket_chain},{decl_expression}})},
		{rvalue, miniparser::Symbols(rvalue, false, {{API}, {DOUBLE},{STRING}, {UNDEFINED}, {math_expression}, 
			{bool_expression}, {call_chain}, {pipe_chain}, {list}, {disc}, {new_expression}, {function}})},
		{attribute_chain, miniparser::Symbols(attribute_chain, false, {{lvalue, DOT, VARIABLE}})},
		{pipe_chain, miniparser::Symbols(attribute_chain, false, {{rvalue, DOT, VARIABLE}})},
		{bracket_chain, miniparser::Symbols(bracket_chain, false, {{lvalue, LB, rvalue, RB}})},
		{call_chain, miniparser::Symbols(call_chain, false, {{rvalue, LP, commalist, RP}, {rvalue, LP, RP},
			{lvalue, LP, commalist, RP},{lvalue, LP, RP}})},
		{commalist, miniparser::Symbols(commalist, true, {{commalist, COMMA, rvalue}, {commalist, COMMA, lvalue}, {lvalue}, {rvalue}})},
		{list, miniparser::Symbols(list, false, {{LB, commalist, RB}, {NIL}})},
		{disc, miniparser::Symbols(disc, false, {{LBB, colonlist, RBB}, {LBB, RBB}})},
		{colonlist, miniparser::Symbols(colonlist, true, {{colonlistfirst, colonlistlast}})},
		{colonlistfirst, miniparser::Symbols(colonlistfirst, true, {{colonlist, COMMA, VARIABLE}, {VARIABLE}})},
		{colonlistlast, miniparser::Symbols(colonlistlast, true, {{COLON, lvalue}, {COLON, rvalue}})},

	 	{EOI, miniparser::Symbols(EOI, false, {})},
	 	{SEMI, miniparser::Symbols(SEMI, false, {})},
		{COMMA, miniparser::Symbols(COMMA, false, {})},
		{TIMES, miniparser::Symbols(TIMES, false, {}, 40)},
		{LP, miniparser::Symbols(LP, false, {}, 100)},
		{RP, miniparser::Symbols(RP, false, {}, 100)},
		{PLUS, miniparser::Symbols(PLUS, false, {}, 37)},
		{VAR, miniparser::Symbols(VAR, false, {})},
		{EQUAL, miniparser::Symbols(EQUAL, false, {})},
		{FUNCTION, miniparser::Symbols(FUNCTION, false, {})},
		{LB, miniparser::Symbols(LB, false, {})},
		{RB, miniparser::Symbols(RB, false, {})},
		{LBB, miniparser::Symbols(LBB, false, {})},
		{RBB, miniparser::Symbols(RBB, false, {})},
		{COLON, miniparser::Symbols(COLON, false, {})},
		{DOT, miniparser::Symbols(DOT, false, {}, 50)},
		{IF, miniparser::Symbols(IF, false, {})},
		{WHILE, miniparser::Symbols(WHILE, false, {})},
		{FOR, miniparser::Symbols(FOR, false, {})},
		{BREAK, miniparser::Symbols(BREAK, false, {})},
		{CONTINUE, miniparser::Symbols(CONTINUE, false, {})},
		{ISEQUAL, miniparser::Symbols(ISEQUAL, false, {}, 17)},
		{ISLARGER, miniparser::Symbols(ISLARGER, false, {}, 20)},
		{ISSMALLER, miniparser::Symbols(ISSMALLER, false, {}, 19)},
		{ISINEQUAL, miniparser::Symbols(ISINEQUAL, false, {}, 18)},
		{NEW, miniparser::Symbols(NEW, false, {}, 60)},
		{THIS, miniparser::Symbols(THIS, false, {})},
		{OR, miniparser::Symbols(OR, false, {}, 28)},
		{AND, miniparser::Symbols(AND, false, {}, 29)},
		{NOT, miniparser::Symbols(NOT, false, {}, 30)},
		{MINUS, miniparser::Symbols(MINUS, false, {}, 38)},
		{DIVIDE, miniparser::Symbols(DIVIDE, false, {}, 39)},
		{TFB, miniparser::Symbols(TFB, false, {})},
		{UNDEFINED, miniparser::Symbols(UNDEFINED, false, {})},
		{NIL, miniparser::Symbols(NIL, false, {})},
		{VARIABLE, miniparser::Symbols(VARIABLE, false, {})},
		{DOUBLE, miniparser::Symbols(DOUBLE, false, {})},
		{STRING, miniparser::Symbols(STRING, false, {})},
		{API, miniparser::Symbols(API, false, {})}
	};

	std::map<std::pair<int, int>, std::function<void(std::deque<boost::any>&)>> functionMap = {
		{{program, 0}, function0_0},
		{{expressionlist, 0}, function1_0},
		{{expressionlist, 1}, function1_1},
		{{expression, 0}, std::bind(function2_0, std::placeholders::_1, math_e)},
		{{expression, 1}, std::bind(function2_0, std::placeholders::_1, assign_e)},
		{{expression, 2}, std::bind(function2_0, std::placeholders::_1, bool_e)},
		{{expression, 3}, std::bind(function2_0, std::placeholders::_1, callchain)},
		{{expression, 4}, std::bind(function2_0, std::placeholders::_1, lvalue_l)},
		{{expression, 5}, std::bind(function2_0, std::placeholders::_1, decl_e)},
		{{expression, 6}, std::bind(function2_0, std::placeholders::_1, bbrk)},
		{{expression, 7}, std::bind(function2_0, std::placeholders::_1, ccont)},
		{{expression, 8}, std::bind(function2_0, std::placeholders::_1, rtrn_e)},
		{{expression, 9}, std::bind(function2_0, std::placeholders::_1, while_e)},
		{{expression, 10}, std::bind(function2_0, std::placeholders::_1, if_e)},
		{{expression, 11}, std::bind(function2_0, std::placeholders::_1, for_e)},
		{{expression, 12}, function2_1},
		{{assign_expression, 0}, function3_0},
		{{assign_expression, 1}, function3_0},
		{{math_expression, 0}, std::bind(function4_0, std::placeholders::_1, minus)},
		{{math_expression, 1}, std::bind(function4_0, std::placeholders::_1, plus)},
		{{math_expression, 2}, std::bind(function4_0, std::placeholders::_1, times)},
		{{math_expression, 3}, std::bind(function4_0, std::placeholders::_1, divide)},
		{{math_expression, 4}, std::bind(function4_0, std::placeholders::_1, minus)},
		{{math_expression, 5}, std::bind(function4_0, std::placeholders::_1, plus)},
		{{math_expression, 6}, std::bind(function4_0, std::placeholders::_1, times)},
		{{math_expression, 7}, std::bind(function4_0, std::placeholders::_1, divide)},
		{{math_expression, 8}, function4_1},
		{{math_expression, 9}, std::bind(function4_2, std::placeholders::_1, lvalue_l)},
		{{math_expression, 10}, std::bind(function4_2, std::placeholders::_1, double_c)},
		{{math_expression, 11}, std::bind(function4_2, std::placeholders::_1, string_c)},
		{{bool_expression, 0}, function5_0},
		{{bool_expression, 1}, function5_1},
		{{bool_expression, 2}, std::bind(function5_2, std::placeholders::_1, and_b)},
		{{bool_expression, 3}, std::bind(function5_2, std::placeholders::_1, or_b)},
		{{bool_expression, 4}, std::bind(function5_2, std::placeholders::_1, islarger_b)},
		{{bool_expression, 5}, std::bind(function5_2, std::placeholders::_1, issmaller_b)},
		{{bool_expression, 6}, std::bind(function5_2, std::placeholders::_1, and_b)},
		{{bool_expression, 7}, std::bind(function5_2, std::placeholders::_1, or_b)},
		{{bool_expression, 8}, std::bind(function5_2, std::placeholders::_1, islarger_b)},
		{{bool_expression, 9}, std::bind(function5_2, std::placeholders::_1, issmaller_b)},
		{{bool_expression, 10}, std::bind(function5_2, std::placeholders::_1, isinequal_b)},
		{{bool_expression, 11}, std::bind(function5_2, std::placeholders::_1, isinequal_b)},
		{{bool_expression, 12}, std::bind(function5_2, std::placeholders::_1, isequal_b)},
		{{bool_expression, 13}, std::bind(function5_2, std::placeholders::_1, isequal_b)},
		{{bool_expression, 14}, function5_3},
		{{bool_expression, 15}, function5_4},
		{{decl_expression, 0}, function6_0},
		{{new_expression, 0}, function7_0},
		{{new_expression, 1}, function7_1},
		{{return_expression, 0}, function8_0},
		{{return_expression, 1}, function8_1},
		{{while_expression, 0}, function9_0},
		{{if_expression, 0}, function10_0},
		{{for_expression, 0}, function11_0},
		{{function, 0}, function12_0},
		{{function, 1}, function12_1},
		{{lvalue, 0}, function13_0},
		{{lvalue, 1}, function13_1},
		{{lvalue, 2}, std::bind(function13_2, std::placeholders::_1, attrchain)},
		{{lvalue, 3}, std::bind(function13_2, std::placeholders::_1, brcschain)},
		{{lvalue, 4}, std::bind(function13_2, std::placeholders::_1, decl_e)},
		{{rvalue, 0}, function14_0},
		{{rvalue, 1}, function14_1},
		{{rvalue, 2}, function14_2},
		{{rvalue, 3}, function14_3},
		{{rvalue, 4}, std::bind(function14_4, std::placeholders::_1, math_e)},
		{{rvalue, 5}, std::bind(function14_4, std::placeholders::_1, bool_e)},
		{{rvalue, 6}, std::bind(function14_4, std::placeholders::_1, callchain)},
		{{rvalue, 7}, std::bind(function14_4, std::placeholders::_1, pipechain)},
		{{rvalue, 8}, std::bind(function14_4, std::placeholders::_1, list_l)},
		{{rvalue, 9}, std::bind(function14_4, std::placeholders::_1, disc_l)},
		{{rvalue, 10}, std::bind(function14_4, std::placeholders::_1, new_e)},
		{{rvalue, 11}, std::bind(function14_4, std::placeholders::_1, function_l)},
		{{attribute_chain, 0}, function15_0},
		{{pipe_chain, 0}, function16_0},
		{{bracket_chain, 0}, function17_0},
		{{call_chain, 0}, function18_0},
		{{call_chain, 1}, function18_1},
		{{call_chain, 2}, function18_0},
		{{call_chain, 3}, function18_1},
		{{commalist, 0}, function19_0},
		{{commalist, 1}, function19_0},
		{{commalist, 2}, function19_1},
		{{commalist, 3}, function19_1},
		{{list, 0}, function20_0},
		{{list, 1}, function20_1},
		{{disc, 0}, function21_0},
		{{disc, 1}, function21_1},
		{{colonlist, 0}, function22_0},
		{{colonlistfirst, 0}, function23_0},
		{{colonlistfirst, 1}, function23_1},
		{{colonlistlast, 0}, function24_0},
		{{colonlistlast, 1}, function24_0}
	};

	class AST
	{
	public:
		std::shared_ptr<Node> astRoot;
		AST() {
			std::unique_ptr<Lexer> mini(new miniparser::Lexer());
			mini->initMacro({
				{std::string("AROP"), std::string("[+-/\\*]")},
				{std::string("MISCEL0"), std::string("==|&&|!=|\\|\\|")},
				{std::string("MISCEL1"), std::string("=|,|;|\\.|:|\\(|\\)|>|<|!|\\[|\]|\\{|\\}")},
				{std::string("API"), std::string("console.log")},
				{std::string("VARIABLE"), std::string("^([[:alpha:]])+(([[:alnum:]])*)")},
				{std::string("STRING"), std::string("\'[^\']*\'")},
				{std::string("DOUBLE"), std::string("^[-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?")},
			});

			mini->addRegularExprs({
				std::string("{API}"),
				std::string("{STRING}"),
				std::string("{VARIABLE}"),
				std::string("{DOUBLE}"),
				std::string("{MISCEL0}"),
				std::string("{MISCEL1}"),
				std::string("{AROP}"),
				std::string(".")
			});

			std::unique_ptr<LRGotoTableEmitter> minitable(new LRGotoTableEmitter());
			minitable->crunchRules(symbolMap);
			std::cerr<<"#############  total states: "<<minitable->gototable.size()<<"   #############"<<std::endl;
			LRParser parser(std::move(mini), std::move(minitable));
		    parser.parse(symbolMap, functionMap);

			astRoot = boost::any_cast<std::shared_ptr<Node>>(attributeForParentNode);
			astRoot->kind = root;
		}
		~AST() = default;
		static void printASTTree(std::shared_ptr<Node> tree, int blank = 0) {
			for (int i = 0; i < blank; ++i)
			{
				std::cerr<<' ';
			}
			std::cerr<<" Type: "<<tree->kind;
			std::cerr<<" Data: "<<tree->data<<std::endl;

			for (auto const& e: tree->branch)
			{
				printASTTree(e, blank+4);
			}
		}
	};
}
