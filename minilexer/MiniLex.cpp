#include "MiniLex.hpp"

namespace minilex {
	std::string yytext = "";
	std::string pyytext[YYTEXTBUFF];
	std::string::size_type yyleng = 0;
	std::istream& yyin = std::cin;
	std::ostream& yyout = std::cout;
	std::map<std::string, int> keywords = {
		{";", 1},
		{",", 2},
		{"*", 3},
		{"(", 4},
		{")", 5},
		{"+", 6},
		{"var", 7},
		{"=", 9},
		{"function", 10},
		{"[", 11},
		{"]", 12},
		{"{", 13},
		{"}", 14},
		{":", 15},
		{".", 16},
		{"if", 17},
		{"while", 18},
		{"for", 19},
		{"break", 20},
		{"continue", 21},
		{"==", 22},
		{">", 23},
		{"<", 24},
		{"!=", 25},
		{"new", 26},
		{"this", 27},
		{"||", 28},
		{"&&", 29},
		{"!", 30},
		{"-", 31},
		{"/", 32},
		{"&", 33},
		{"true", 34},
		{"false", 34},
		{"undefined", 35},
		{"nil", 36},
		{"return", 37},
		{"console.log", 101} // Build-in API, all api equals 101
	};

	int yyline = 0;
	
	void MiniLex::initMacro(std::initializer_list<std::pair<std::string, std::string> > il) {
		std::unique_ptr<minilex::MacroHandler> uq(new minilex::MacroHandler());
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			uq->addMacro(*it);
		}

		std::unique_ptr<RegularExp> uqq(new RegularExp(std::move(uq)));

		reup.reset(uqq.release());
	}

	void MiniLex::addRegularExprs(std::initializer_list<std::string > il) {
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			reup->addRegularExp(*it);
		}
	}

	void MiniLex::removeRegularExprs(std::initializer_list<std::string > il) {
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			reup->removeRegularExp(*it);
		}
	}

	int MiniLex::yylex() {
		std::string eaten;
		std::string pattern;
		if(yyin.eof() && linebuffer.empty() && flushed == false)
		{
			std::cerr<<"EOF at line "<<yyline<<std::endl;
			return 0;
		}

		flushed = false;
		if(linebuffer.empty())
		{
			yyline++;
			std::getline(yyin, linebuffer);
		}

		eaten = reup->eat(linebuffer);
		yyleng = eaten.size();
		//pyytext = yytext;
		yytext = eaten;
		if(!reup->isEaten() && !yyin.eof())
		{
			yyin.clear();
			flushed = true;
			std::cerr<<"IGNORE STRING at line "<<yyline<<" "<<linebuffer<<std::endl;
			return 8;
		}

		pattern = reup->matchPattern();

        /* you are required to modify the following code for your own purposes */
        if(pattern == std::string("{MISCEL0}")) {
			std::cerr<<"RECOGNIZE MISCEL0 at line "<<yyline<<' '<<eaten<<' '<<keywords[yytext]<<std::endl;
			return keywords[yytext];
		}
		else if(pattern == std::string("{API}")) {
			std::cerr<<"RECOGNIZE API at line "<<yyline<<' '<<eaten<<std::endl;
			return keywords[yytext];
		}
		else if(pattern == std::string("{STRING}")) {
			std::cerr<<"RECOGNIZE STRING LITERAL at line "<<yyline<<' '<<eaten<<std::endl;
			return 98;
		}
		else if(pattern == std::string("{MISCEL1}")) {
			std::cerr<<"RECOGNIZE MISCEL1 at line "<<yyline<<' '<<eaten<<std::endl;
			return keywords[yytext];
		}
		else if(pattern == std::string("{AROP}")) {
			std::cerr<<"RECOGNIZE AROP at line "<<yyline<<' '<<eaten<<std::endl;
			return keywords[yytext];
		}
		else if(pattern == std::string("{DOUBLE}")) {
			std::cerr<<"RECOGNIZE DOUBLE at line "<<yyline<<' '<<eaten<<std::endl;
			return 100;
		}
		else if(pattern == std::string("{VARIABLE}")) {
			try {
				std::cerr<<"try keywords at line "<<yyline<<' '<<eaten<<std::endl;
				return keywords.at(yytext);
			}
			catch(...)
			{
				std::cerr<<"RECOGNIZE VARIABLE at line "<<yyline<<' '<<eaten<<std::endl;
				return 99;
			}
		}
		else if(pattern == std::string(".")) {
			std::cerr<<"UNRECOGNIZE at line"<<yyline<<' '<<eaten<<std::endl;
			return 8;
		}
		
		return 0;
	}
}
