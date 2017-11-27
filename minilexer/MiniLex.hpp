#pragma once
#include <iostream>
#include "RegularExp.hpp"

#define YYTEXTBUFF	100

namespace minilex {
	extern std::string yytext;
	extern std::string pyytext[YYTEXTBUFF];
	extern std::string::size_type yyleng;
	extern std::istream& yyin;
	extern std::ostream& yyout;
	extern int yyline;
	extern std::map<std::string, int> keywords; 

	class MiniLex
	{
	private:
		int flushed = false;
		std::unique_ptr<RegularExp> reup;
		std::string linebuffer;
	public:
		MiniLex() = default;
		~MiniLex() = default;
		int yylex();
		void initMacro(std::initializer_list<std::pair<std::string, std::string> > il);
		void addRegularExprs(std::initializer_list<std::string > il);
		void removeRegularExprs(std::initializer_list<std::string > il);

	};
}