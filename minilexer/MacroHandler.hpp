#pragma once
#include <map>

namespace minilex {
	
	class MacroHandler
	{
	private:
		std::map<std::string, std::string> macrotab;

	public:
		MacroHandler() = default;
		~MacroHandler() = default;
		std::string expandMacro(const std::string& macroname);
		void addMacro(std::pair<std::string, std::string> macro);
	};
}
