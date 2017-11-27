#include "MiniLex.hpp" // you are required to modify yylex() for your own purposes in MiniLex.cpp 

int main(int argc, char const *argv[])
{
	std::unique_ptr<minilex::MiniLex> mini(new minilex::MiniLex());
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

	while(mini->yylex());
	return 0;
}