#pragma once
#include <map>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "GrammarState.hpp"

namespace miniparser {

    class LRGotoTableEmitter
    {
    public:
    	std::map<int, std::map<int, std::tuple<int, int, int>>> gototable;
    public:
    	LRGotoTableEmitter() = default;

    	~LRGotoTableEmitter() = default;

        void crunchRules(std::map<int, Symbols>& symp) {
            std::ifstream ifile("GOTOTABLE.map");
            if(!ifile) {
                miniparser::FirstSet first(symp);
                first.runFirstSets(symp);
                miniparser::ProductionManager pm;
                pm.initProductions(symp);
                miniparser::GrammarGraph gg;
                gg.buildTransitionStateMachine(pm, symp);
                miniparser::GotoTable gt;
                gt.constructGotoTable(gg, symp);
                gototable.swap(gt.gototable);
                std::ofstream ofile("GOTOTABLE.map");
                for (auto const& x: gototable)
                {
                    for (auto const& y: x.second)
                    {
                        ofile<<std::setw(4)<<x.first<<std::setw(4)<<y.first<<std::setw(4)<<std::get<0>(y.second)<<std::setw(4)<<std::get<1>(y.second)<<std::setw(4)<<std::get<2>(y.second)<<std::endl;
                    }
                }
                ofile<<std::setw(4)<<-1<<std::setw(4)<<-1<<std::setw(4)<<-1<<std::setw(4)<<-1<<std::setw(4)<<-1;
                ofile.close();
            }
            else
            {
                int e[5] = {0,};
                std::string line = "";
                while(!ifile.eof()) {
                    line.clear();
                    getline(ifile, line);
                    for(int i = 0; i < 5; i++) {
                        std::string r = line.substr(4 * i, 4);
                        e[i] = std::stoi(r);
                    }
                    gototable[e[0]][e[1]] = {e[2],e[3],e[4]};
                }

                ifile.close();
            }
        }

        std::tuple<int, int, int> getAction(int state, int symbol) {
            try {
                return gototable.at(state).at(symbol);
            }
            catch(...)
            {
                return {-1, -1, -1};
            }
        }
    };
}
