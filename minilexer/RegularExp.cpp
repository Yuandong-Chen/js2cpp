#include <iostream>
#include "RegularExp.hpp"

namespace minilex {
	RegularExp::RegularExp(std::unique_ptr<MacroHandler>&& macrorp) {
		macrohp.reset(macrorp.release());
	}

	std::string RegularExp::expandRegularExp(const std::string& rexp) {
		bool inquoted = false;
		std::string ret = "";
		int i;
		char c;
		char prevc = '\0';
		auto max = std::distance(rexp.cbegin(), rexp.cend());
		for (i = 0; i < max; ++i)
		{
			c = rexp[i];
			if(isspace(c))
			{
				ret += c;
				continue;
			}

			if(c == '"' && prevc != '\\' ) {
				inquoted = !inquoted;
				prevc = c;
				continue;
			}

			if(!inquoted && c == '{' && prevc != '\\') {
				std::string name = RegularExp::extractMacroName(rexp, i, max);
				ret += RegularExp::expandMacro(name);
			}
			else
			{
				ret += c;
			}

			prevc = c;
		}

		return ret;
	}

	std::string RegularExp::extractMacroName(const std::string& rexp, int &i, int max) {
		std::string name = "";
		bool inquoted = false;
		char c;
		char prevc = '\0';
		i++;
		for (; i < max; ++i)
		{
			c = rexp[i];
			if( c == '"' && prevc != '\\')
			{
				inquoted = !inquoted;
			}

			if(!inquoted && c == '}' && prevc != '\\')
			{
				return name;
			}

			name += c;
			prevc = c;
		}

		std::cerr<<"FATAL: cannot extract MACRO"<<std::endl;
		exit(1);
		return "";
	}

	std::string RegularExp::expandMacro(const std::string& macroname) {
		std::string macroContent = macrohp->expandMacro(macroname);
		bool inquoted = false;
		bool switchcontent = false;
		int i = 0;
		std::string content = "";
		std::string neededexpand = "";
		char c = macroContent[0];
		char prevc = '\0';

		while(i < macroContent.length())
		{
			if(c == '"' && prevc != '\\')
			{
				inquoted = !inquoted;
			}

			if(!inquoted && c == '{' && prevc != '\\')
			{
				switchcontent = !switchcontent;
				i++;
				prevc = c;
				c = macroContent[i];
				continue;
			}

			if(!inquoted && c == '}' && prevc != '\\')
			{
				content += RegularExp::expandMacro(neededexpand);
				neededexpand = "";
				switchcontent = !switchcontent;
				i++;
				prevc = c;
				c = macroContent[i];
				continue;
			}

			if(!switchcontent)
			{
				content += c;
			}
			else {
				neededexpand += c;
			}

			i++;
			prevc = c;
			c = macroContent[i];
		}

		return content;
	}

	void RegularExp::addRegularExp(std::string rexp) {
		std::string pattern = RegularExp::expandRegularExp(rexp);
		try {
			regularexps.emplace_back(std::move(std::unique_ptr<std::regex>(new std::regex(pattern, std::regex_constants::extended))), rexp);	
		}
		catch(std::regex_error e)
		{
			std::cerr<< e.what() <<std::endl;
			exit(1);
		}
	}

	void RegularExp::removeRegularExp(std::string rexp) {
		auto it = regularexps.begin();
		for (; it != regularexps.end(); ++it)
		{
			if((*it).second == rexp )
			{
				(*it).first.reset();
				it = regularexps.erase(it);
			}
		}
	}
	
	std::string RegularExp::eat(std::string& txt) {
		int i;
		std::string ret = "";
		auto it = regularexps.begin();
		std::smatch results;

		for (; it != regularexps.end(); ++it)
		{
			if(std::regex_search(txt, results, *((*it).first), std::regex_constants::match_continuous))
			{
				success = true;
				ret = results.str();
				txt = txt.substr(ret.size());
				currentMatch = (*it).second;
				return ret;
			}
		}

		success = false;
		currentMatch = "";
		return "";
	}
}
