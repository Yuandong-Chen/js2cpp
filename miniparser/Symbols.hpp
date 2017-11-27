#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <boost/any.hpp>
#include "../minilexer/MiniLex.hpp"

#define TERMINAL_N 256

namespace miniparser {
	enum Symbol_N {
		// define your symbols
		program = TERMINAL_N,
		UNKNOWN_SYMBOL = 8,
		EOI = 0,
		lvalue = 266,
		rvalue = 267,
		list = 268,
		disc = 269,
		colonlist = 270,
		attribute_chain = 271,
		call_chain = 272,
		function = 273,
		commalist = 274,
		expressionlist = 275,
		expression = 276,
		while_expression = 277,
		for_expression = 278,
		if_expression = 279,
		assign_expression = 280,
		return_expression = 281,
		new_expression = 282,
		math_expression = 283,
		bool_expression = 284,
		decl_expression = 285,
		bracket_chain = 286,
		pipe_chain = 287,
		colonlistfirst = 288,
		colonlistlast = 289,
		
		SEMI = 1,
		COMMA = 2,
		TIMES = 3,
		LP = 4,
		RP = 5,
		PLUS = 6,
		VAR = 7,
		EQUAL = 9,
		FUNCTION = 10,
		LB = 11,
		RB = 12,
		LBB = 13,
		RBB = 14,
		COLON = 15,
		DOT = 16,
		IF = 17,
		WHILE = 18,
		FOR = 19,
		BREAK = 20,
		CONTINUE = 21,
		ISEQUAL = 22,
		ISLARGER = 23,
		ISSMALLER = 24,
		ISINEQUAL = 25,
		NEW = 26,
		THIS = 27,
		OR = 28,
		AND = 29,
		NOT = 30,
		MINUS = 31,
		DIVIDE = 32,
		SINGLEAND = 33,
		TFB = 34,
		UNDEFINED = 35,
		NIL = 36,
		RETURN = 37,
		VARIABLE = 99,
		DOUBLE = 100,
		STRING = 98,
		API = 101
	};

	class Symbols
	{
	public:
		int value;
		std::vector<std::vector<int> > productions;
		std::vector<int> firstSet;
		bool isNullable;
		int priority = -1;

		Symbols(int symVal, bool nullable, std::vector<std::vector<int> > pro, int p = 0) {
			value = symVal;
			productions = pro;
			isNullable = nullable;
			if(symVal < TERMINAL_N)
			{
				priority = p;
				firstSet.push_back(symVal);
			}
		};
		~Symbols() = default;

		bool contains(int e){
			return (std::find(firstSet.begin(), firstSet.end(), e) != firstSet.end());
		};

		bool containsAll(const std::vector<int> other) {
			if(firstSet.size() < other.size())
				return false;

			for (int i = 0; i < other.size(); ++i)
			{
				if(std::find(firstSet.begin(), firstSet.end(), other[i]) == firstSet.end())
					return false;
			}
			return true;
		}
		
	};

	boost::any attributeForParentNode;
}


