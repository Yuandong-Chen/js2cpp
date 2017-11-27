#include "MacroHandler.hpp"
#include <iostream>
#include <functional>
namespace minilex {

	std::string MacroHandler::expandMacro(const std::string& macroname) {
		std::map<std::string, std::string>::iterator iter;
		if(macrotab.find(macroname) == macrotab.end()){

		}
		else {
			std::string ret = "";
			ret += "(" + macrotab[macroname] + ")";
			return ret;
		}

		return "";	
	}

	void MacroHandler::addMacro(std::pair<std::string, std::string> macro) {
		macrotab.insert(macro);
	}	
}
