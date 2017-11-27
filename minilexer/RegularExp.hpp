#pragma once
#include <functional>
#include <list>
#include <regex>
#include "MacroHandler.hpp"

namespace minilex {
	
	class RegularExp
	{
	private:
		bool success = false;
		std::string currentMatch = "";
		std::unique_ptr<MacroHandler> macrohp;
		std::list<std::pair<std::unique_ptr<std::regex>, std::string> > regularexps;
	public:
		std::string expandRegularExp(const std::string& rexp);
		std::string extractMacroName(const std::string& rexp, int &index, int max);
		std::string expandMacro(const std::string& macroname);
	public:
		RegularExp(std::unique_ptr<MacroHandler>&& macrorp);
		~RegularExp() = default;
		void addRegularExp(std::string rexp);
		void removeRegularExp(std::string rexp);
		std::string eat(std::string& txt);
		bool isEaten(){return success;};
		std::string matchPattern(){return currentMatch;};
	};

}