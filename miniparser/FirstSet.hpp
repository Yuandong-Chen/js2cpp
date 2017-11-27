#pragma once
#include "Symbols.hpp"

namespace miniparser {
	class FirstSet
	{
	private:
		bool runFirstSetPass = true;

		std::vector<std::reference_wrapper<Symbols> > symbolArray;

		bool isSymbolTerminals(int symbol) {
			return symbol < TERMINAL_N;
		};

	public:
		FirstSet(std::map<int, Symbols>& symp) {
			auto it = symp.begin();
			for (; it != symp.end(); ++it)
			{
				symbolArray.push_back(it->second);
			}
		};

		~FirstSet() = default;

		void runFirstSets(std::map<int, Symbols>& symp) {
			while(runFirstSetPass) {
				runFirstSetPass = false;

				auto it = symbolArray.begin();
				for (; it != symbolArray.end(); ++it)
				{
					Symbols& symbol = it->get();
					addSymbolFirstSet(symbol, symp);
				}
			}
		}

		void addSymbolFirstSet(Symbols& symbol, std::map<int, Symbols>& symp) {
			if (isSymbolTerminals(symbol.value) == true) {
    			return;
    		}
    		for (int i = 0;  i < symbol.productions.size(); i++) {
    			auto rightSize = symbol.productions[i];
    			if (isSymbolTerminals(rightSize[0]) && symbol.contains(rightSize[0]) == false) {
    				runFirstSetPass = true;
    				symbol.firstSet.push_back(rightSize[0]);
    			}
    			else if (isSymbolTerminals(rightSize[0]) == false) {
    				//add first set of nullable
    				int pos = 0;
    				do {
    					Symbols& curSymbol = symp.find(rightSize[pos])->second;
    					if (symbol.containsAll(curSymbol.firstSet) == false) {
    						runFirstSetPass = true;
    						
    						for (int j = 0; j < curSymbol.firstSet.size(); j++) {
    							if (symbol.contains(curSymbol.firstSet[j]) == false) {
    								symbol.firstSet.push_back(curSymbol.firstSet[j]);
    							}
    						}//for (int j = 0; j < curSymbol.firstSet.size(); j++)
    						
    					}//if (symbol.firstSet.containsAll(curSymbol.firstSet) == false)
    					
    					pos++;
    					if(!curSymbol.isNullable)
    						break;
    				}while(pos < rightSize.size());
    			} // else
    		}//for (int i = 0; i < symbol.productions.size(); i++)
		}	
	};
}


