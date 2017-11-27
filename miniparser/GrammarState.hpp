#pragma once
#include "Production.hpp"
#include <stack>
#include <boost/range/adaptor/reversed.hpp>
#include <tuple>
namespace miniparser {
	class GrammarState
	{	
	public:
		int stateNum = -1;
		bool printInfo = false;
		std::vector<Production> productions; // == in c++11 specification for sequence container

		bool transitionDone = false;
		std::vector<Production> closureSet; 
		std::map<int, std::vector<Production>> partition;

		static int stateNumCount;

		GrammarState() {

		}

		GrammarState(const std::vector<Production>& productions) {
			stateNum = stateNumCount;
			this->productions = productions;
		}

		GrammarState(std::vector<Production>&& productions) {
			stateNum = stateNumCount;
			this->productions = productions;
		}

		GrammarState(const GrammarState& gs) {
			stateNum = stateNumCount;
			this->productions = gs.productions;
		}

		GrammarState(GrammarState&& gs) {
			stateNum = stateNumCount;
			this->productions = gs.productions;
		}

		GrammarState& operator= (const std::vector<Production>& productions) {
			stateNum = stateNumCount;
			this->productions = productions;
			return *this;
		}

		GrammarState& operator= (std::vector<Production>&& productions) {
			stateNum = stateNumCount;
			this->productions = productions;
			return *this;
		}

		GrammarState& operator= (const GrammarState& gs) {
			stateNum = stateNumCount;
			this->productions = gs.productions;
			return *this;
		}

		GrammarState& operator= (GrammarState&& gs) {
			stateNum = stateNumCount;
			this->productions = gs.productions;
			return *this;
		}

		~GrammarState() = default;
		static void increateStateNum() {
			//printf("state num : %d\n",stateNumCount );
			stateNumCount++;
		}

		bool operator== (const GrammarState& g) const {
			return operator== (std::move(g));
		}

		bool operator== (const GrammarState&& g) const {
			return this->productions == g.productions;
		}

		bool closureSetContains(const Production& p) {
			return std::find(closureSet.begin(), closureSet.end(), p) != closureSet.end();
		}

		/* possible equal */
		bool closureSetSimilarContains(const Production& p) {
			for (auto const& x: closureSet)
			{
				if(x.isSimilar(p))
				{
					return true;
				}
			}

			return false;
		}
	};

	int GrammarState::stateNumCount = 0;

	#define GRAPHSIZE 10000
	class GrammarGraph
	{
	private:
		
	public:
		GrammarState stateList[GRAPHSIZE]; // fetch element in sequence, implicit state num as index
		std::map<int, std::map<int, int>> transition; //{statenum, <symbol, statenum>}

		GrammarGraph() = default;
		~GrammarGraph() = default;

		bool stateContains(const GrammarState& gs) {
			return std::find(std::begin(stateList), std::end(stateList), gs) != std::end(stateList);
		}
		/* return reference */
		GrammarState& getGrammarState(std::vector<Production>&& productionList) {
			for (int i = 0; i < GRAPHSIZE; ++i)
			{
				if(stateList[i] == GrammarState(productionList))
				{
					return stateList[i];
				}
			}

			stateList[GrammarState::stateNumCount] = std::move(productionList);
			GrammarState::increateStateNum();
			return stateList[GrammarState::stateNumCount - 1];
		}

		void buildTransitionStateMachine(ProductionManager& pm, std::map<int, Symbols>& symp) {
			GrammarState& state = getGrammarState(pm.getProduction(TERMINAL_N));
			for (auto& x: state.productions)
			{
				x.lookAhead.push_back(EOI);
			}

			createTransition(state, pm, symp);
		}

		void createTransition(GrammarState& node, ProductionManager& pm, std::map<int, Symbols>& symp) {
			if(node.transitionDone == true) {
				return;
			}

			node.transitionDone = true;

			makeClosure(node, pm, symp);
			partition(node);
			makeTransition(node, pm);
			dfsExtendTransition(node, pm, symp);
		}

		void sort(std::vector<int>& v) {
			std::sort(v.begin(), v.end());
		}

		std::vector<int> unionize(std::vector<int>& left, std::vector<int>& right) {
			std::vector<int> v1 = left;
			std::vector<int> v2 = right;
			std::vector<int> ret;
			ret.resize(left.size() + right.size());
			auto it = std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), ret.begin());
			ret.resize(std::distance(ret.begin(), it));
			return ret;
		}

		std::vector<int> First_S(std::vector<int>& beta, std::vector<int>& C, std::map<int, Symbols>& symp) {
			std::vector<int> ret;
			int i = 0;
			while(i < beta.size()) {
				if(symp.at(beta[i]).isNullable) {
					this->sort(ret);
					this->sort(symp.at(beta[i]).firstSet);
					ret = this->unionize(symp.at(beta[i]).firstSet,ret);
				}
				else
				{
					this->sort(ret);
					this->sort(symp.at(beta[i]).firstSet);
					ret = this->unionize(symp.at(beta[i]).firstSet,ret);
					return ret;
				}
				i++;
			}

			this->sort(ret);
			this->sort(C);
			ret = this->unionize(C,ret);
			return ret;
		}

		bool contains(const std::vector<int>& left, const std::vector<int>& right) {
			// if left contains right
			return std::includes(left.begin(), left.end(),right.begin(), right.end());
		}

		bool trySimilar(Production& known, Production& potential, 
			std::stack<Production>& stack, GrammarState& node, std::map<int, Symbols>& symp) {

			if(node.closureSetSimilarContains(potential) == true && node.closureSetContains(potential) == false) {

				for (auto& x: node.closureSet)
				{
					if(x.isSimilar(potential))
					{
						this->sort(x.lookAhead);
						this->sort(potential.lookAhead);
						std::vector<int> beta;
						std::vector<int> C = known.lookAhead;
						int dot = known.getDotPosition();
						for (int i = dot + 1; i < known.right.size(); ++i)
						{
							beta.push_back(known.right[i]);
						}
						potential.lookAhead = this->First_S(beta, C, symp);

						if(this->contains(x.lookAhead, potential.lookAhead) == false) {
							x.lookAhead = this->unionize(x.lookAhead, potential.lookAhead);
							stack.push(potential);
						}

						return true;
					}
				}
			}

			return false;
		}

		void addressEqualOrDiff(Production& known, Production& potential, 
			std::stack<Production>& stack, GrammarState& node, std::map<int, Symbols>& symp) {
			if(node.closureSetContains(potential) == false) {
				std::vector<int> beta;
				std::vector<int> C = known.lookAhead;
				int dot = known.getDotPosition();
				for (int i = dot + 1; i < known.right.size(); ++i)
				{

					beta.push_back(known.right[i]);
				}
				potential.lookAhead = this->First_S(beta, C, symp);
				node.closureSet.push_back(potential);
				stack.push(potential);

			}
		}

		void makeClosure(GrammarState& node, ProductionManager& pm, std::map<int, Symbols>& symp) {
			node.closureSet = node.productions;
			std::stack<Production> productionStack;
			for (int i = 0; i < node.closureSet.size(); ++i)
			{
				productionStack.push(node.closureSet.at(i));
			}
			while(productionStack.empty() == false) {
				Production production = productionStack.top();
				productionStack.pop();
				int symbol = production.getDotSymbol();
				std::vector<Production> closures = pm.getProduction(symbol);

				for (int i = 0; i < closures.size(); ++i)
				{
					if(trySimilar(production, closures.at(i), productionStack, node, symp) == false) {
						addressEqualOrDiff(production, closures.at(i), productionStack, node, symp);
					}
				}
			}
		}

		void partition(GrammarState& node) {
			for (int i = 0; i < node.closureSet.size(); ++i)
			{
				int symbol = node.closureSet.at(i).getDotSymbol();
				if(symbol == UNKNOWN_SYMBOL) { // reach the end of production
					continue;
				}

				try {
					std::vector<Production>& productionList = node.partition.at(symbol);
					productionList.push_back(node.closureSet.at(i));
				}
				catch(...)
				{
					node.partition.insert({symbol, std::vector<Production>()});
					std::vector<Production>& productionList = node.partition.at(symbol);
					productionList.push_back(node.closureSet.at(i));
				}
			}
		}

		GrammarState& makeNextGrammarState(GrammarState& node, int left) {

			std::vector<Production> productionList = node.partition.at(left);
			std::vector<Production> newStateProductionList;

			for (int i = 0; i < productionList.size(); ++i)
			{
				newStateProductionList.push_back(productionList.at(i).dotForward());
			}

			return getGrammarState(std::move(newStateProductionList));
		}
		
		void makeTransition(GrammarState& node, ProductionManager& pm) {
			
			for (auto const& x: node.partition)
			{
				GrammarState& nextState = makeNextGrammarState(node, x.first);
				transition[node.stateNum].insert({x.first, nextState.stateNum});
			}
		}

		void dfsExtendTransition(GrammarState& node, ProductionManager& pm, std::map<int, Symbols>& symp) {
			for (auto& x: transition[node.stateNum])
			{
				GrammarState& state = stateList[x.second];
				if(state.transitionDone == false) {
					createTransition(state, pm, symp);
				}
			}
		}
	};

	enum LabelGoto { 
		ACCEPT = 0,
		GOTO = 1,
		SHIFT = 2,
		REDUCE = 3,
	};

	class GotoTable
	{
	public:
		//         term           |       term       |  ...
		// state   <Label, num, *>: <GOTO, state, *>, <SHIFT, state, *>, <ACCEPT, *, *>, <REDUCE,TERMINAL,index>
		//std::map<int, std::map<int, int>> transition; //{statenum, <symbol, statenum>}
		std::map<int, std::map<int, std::tuple<int, int, int>>> gototable; // only useful info we needed 
		
		GotoTable() = default;
		~GotoTable() = default;

		bool contains(const std::vector<int>& v, int e) {
			return std::find(v.begin(), v.end(), e) != v.end();
		}

		std::vector<Production> reduceAssociated(int state, int symbol, const GrammarGraph& graph) {
			std::vector<Production> ret;
			for (auto const& x: graph.stateList[state].productions)
			{
				if(this->contains(x.lookAhead, symbol) && x.getDotPosition() == x.right.size()) {
					ret.push_back(x);
				}
			}
			return ret;
		}

		/* return true if choose shift */
		void shiftReduceConflict(int state, int symbol, int gotostate, const GrammarGraph& graph, std::vector<Production>& reduces, const std::map<int, Symbols>& symp) {
			// symbol must be terminal symbol
			for (auto const& x: reduces)
			{
				for (auto const& e: boost::adaptors::reverse(x.right))
				{
					if(e < TERMINAL_N) {
						if(symp.at(e).priority > symp.at(symbol).priority)
						{
							return;
						}
					}
				}
			}
			gototable[state][symbol] = {SHIFT, gotostate, -1};
			/* too special only for lambda expression  */
			//return;
		}

		void reduceReduceConflict(int state, int symbol, std::vector<Production>& reduces, const std::map<int, Symbols>& symp) {
			// choose highest priority reduce production if all equal, choose the longest one
			// std::cerr<<"----------------            Warning            ---------------------"<<std::endl;
			// std::cerr<<"reduce-reduce conflict: you'd better redefine your syntax"<<std::endl;
			// std::cerr<<"we will choose highest priority reduce production, if all equal, choose the longest one"<<std::endl;
			// std::cerr<<"----------------                               ---------------------"<<std::endl;

			std::sort(reduces.begin(), reduces.end(), [=](const Production& x1, const Production& x2){
				int x1p = -1, x2p = -1;
				for (auto const& e: boost::adaptors::reverse(x1.right))
				{
					if(e < TERMINAL_N) {
						x1p = symp.at(e).priority;
						break;
					}
				}

				for (auto const& e: boost::adaptors::reverse(x2.right))
				{
					if(e < TERMINAL_N) {
						x2p = symp.at(e).priority;
						break;
					}
				}
				return ((x1p != x2p)? x1p < x2p: x1.right.size() < x2.right.size());
			});

			Production p = *reduces.rbegin();
			int i = 0;
			for (auto const& x: symp.at(p.left).productions)
			{
				if(x == p.right) {
					break;
				}
				i++;
			}

			gototable[state][symbol] = {REDUCE, p.left, i};
		}

		void preconstructReduce(const GrammarState& state, const std::map<int, Symbols>& symp) {
			std::map<int, std::vector<Production>> reducesmap;

			for (auto const& x: state.productions)
			{
				if(x.getDotPosition() == x.right.size()) {
					for (auto const& i: x.lookAhead)
					{
						reducesmap[i].push_back(x);
					}
				}
			}

			for (auto& x: reducesmap)
			{
				if(x.second.size() > 1) {
					reduceReduceConflict(state.stateNum, x.first, x.second, symp);
				}
				else
				{
					int i = 0;
					Production p = *(x.second.begin());
					for (auto const& j: symp.at(p.left).productions)
					{
						if(j == p.right) {
							break;
						}
						i++;
					}
					gototable[state.stateNum][x.first] = {REDUCE, p.left, i};
				}
			}
		}

		void constructGotoTable(const GrammarGraph& graph, const std::map<int, Symbols>& symp) {
			
			for (int i = 0; i < GrammarState::stateNumCount; ++i)
			{
				preconstructReduce(graph.stateList[i], symp);
			}

			for (auto const& x: graph.transition)
			{
				for (auto const& y: x.second)
				{
					if(y.first >= TERMINAL_N) {
						gototable[x.first].insert({y.first,{GOTO, y.second, -1}});
					}
					else
					{
						std::vector<Production> reduceProductions = reduceAssociated(x.first, y.first, graph);
						if(reduceProductions.size() > 0)
						{
							shiftReduceConflict(x.first, y.first, y.second, graph, reduceProductions, symp);
							//reduceReduceConflict(x.first, y.first, graph, reduceProductions, symp);
						}
						else
						{
							gototable[x.first][y.first] = {SHIFT, y.second, -1};
						}
					}
				}
			}

			// find accept nodes
			for (auto& state: gototable)
			{
				for (auto& x: state.second)
				{
					if(std::get<0>(x.second) == REDUCE)
					{
						if(std::get<1>(x.second) == TERMINAL_N)
						{
							std::get<0>(x.second) = ACCEPT;
						}
					}
				}
			}
		}
	};
}

