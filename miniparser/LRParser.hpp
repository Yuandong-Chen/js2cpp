#pragma once
#include <deque>
#include "LRGotoTableEmitter.hpp"

namespace miniparser {
	class Lexer: public minilex::MiniLex
	{
	public:
		Lexer() {
		}
		~Lexer() = default;
		int lookAhead = -1;
		int advance() {
            minilex::pyytext[0] = minilex::yytext;
            for (int i = 1; i < YYTEXTBUFF; ++i)
            {
                minilex::pyytext[i] = minilex::pyytext[i - 1];
            }
            
			while((lookAhead = minilex::MiniLex::yylex()) == UNKNOWN_SYMBOL){};
            return lookAhead;
		}
	};

	class LRParser
	{
	private:
		std::unique_ptr<Lexer> lexer;
		int lexerInput = 0;
		std::stack<int> statusStack;
        std::deque<boost::any> valueStack;
        std::stack<int> parseStack;
		std::unique_ptr<LRGotoTableEmitter> gotoTable;

	public:
		LRParser(std::unique_ptr<Lexer> l, std::unique_ptr<LRGotoTableEmitter> g) {
            lexer.reset(l.release());
            gotoTable.reset(g.release());
			statusStack.push(0);
            lexer->advance();
			lexerInput = lexer->lookAhead;
		};
		~LRParser() = default;
        
		void parse(const std::map<int, Symbols>& moos, 
            const std::map<std::pair<int, int>, std::function<void(std::deque<boost::any>&)>>& foos) {

			std::tuple<int, int, int> action = gotoTable->getAction(statusStack.top(), lexerInput);
			while (std::get<0>(action) != ACCEPT) {
                //std::cout<<"("<<statusStack.top()<<","<<lexerInput<<")"<<std::endl;
    			action = gotoTable->getAction(statusStack.top(), lexerInput);
                //std::cout<<"action: "<<action<<std::endl;
    			if (std::get<0>(action) == -1) {
    				std::cerr<<"PARSE ERROR at line "<<minilex::yyline;
                    std::cerr<<" after text --'"<<minilex::pyytext[0]<<"'-- we found --'"<<minilex::yytext<<"'-- the token "<<lexerInput<<std::endl;
                    try {
                        std::cerr<<"Expected input tokens: ";
                        for(auto const& x: gotoTable->gototable.at(statusStack.top())) {
                            std::cerr<<" "<<x.first<<" ";
                        }
                        std::cerr<<std::endl;
                    }
                    catch(...)
                    {
                        std::cerr<<" FATAL ERROR NO SUGGESTION "<<std::endl;
                    }
                    
    				return;
    			}
    		
    			if (std::get<0>(action) == ACCEPT) {
    				std::cerr<<"The input can be accepted"<<std::endl;
    				return;
    			}

                if(std::get<0>(action) == SHIFT) {
                    statusStack.push(std::get<1>(action));
                    parseStack.push(lexerInput);
                    valueStack.push_back(nullptr);
                    lexer->advance();
                    lexerInput = lexer->lookAhead;
                    continue;
                }

                if(std::get<0>(action) == REDUCE) {
                    try {
                        foos.at({std::get<1>(action), std::get<2>(action)})(valueStack);
                    }
                    catch(...)
                    {
                        
                    }
                    
                    int size = moos.at(std::get<1>(action)).productions.at(std::get<2>(action)).size();

                    for (int i = 0; i < size; ++i)
                    {
                        statusStack.pop();
                        parseStack.pop();
                        valueStack.pop_back();
                    }

                    lexerInput = std::get<1>(action);
                    parseStack.push(lexerInput);
                    valueStack.push_back(attributeForParentNode);
                    continue;
                }

                if(std::get<0>(action) == GOTO) {
                    statusStack.push(std::get<1>(action));
                    lexerInput = lexer->lookAhead;
                    continue;
                }
                
    		}			
		}
	};
}

