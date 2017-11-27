#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include "FirstSet.hpp"

namespace miniparser {
	class Production
	{
	private:
		int dotPos = 0;
		
	public:
		int left = 0;
		std::vector<int> right;
		std::vector<int> lookAhead;
		Production(int left, int dot, std::vector<int>&& v, std::vector<int>&& v2) {
			this->left = left;
			right.swap(v);
			v.clear();
			lookAhead.swap(v2);
			v2.clear();

			if(dot >= right.size())
			{
				dot = right.size();
			}
			dotPos	= dot;
		}

		Production(int left, int dot, const std::vector<int>& v, const std::vector<int>& v2) {
			this->left = left;
			right = v;
			lookAhead = v2;

			if(dot >= right.size())
			{
				dot = right.size();
			}
			dotPos	= dot;
		}

		Production(Production&& p) {
			this->dotPos = p.dotPos;
			this->left = p.left;
			this->right.swap(p.right);
			p.right.clear();
			this->lookAhead.swap(p.lookAhead);
			p.lookAhead.clear();
		}

		Production(const Production& p) {
			this->dotPos = p.dotPos;
			this->left = p.left;
			this->right = p.right;
			this->lookAhead = p.lookAhead;
		}

		Production& operator= (const Production& p) {
			this->dotPos = p.dotPos;
			this->left = p.left;
			this->right = p.right;
			this->lookAhead = p.lookAhead;
			return *this;
		}

		Production& operator= (Production&& p) {
			this->dotPos = p.dotPos;
			this->left = p.left;
			this->right.swap(p.right);
			p.right.clear();
			this->lookAhead.swap(p.lookAhead);
			p.lookAhead.clear();
			return *this;
		}

		~Production() = default;
		/* this return the rvalue */
		Production dotForward() {
			return Production(left, dotPos + 1, right, lookAhead);
		}

		int getLeft() const{
			return this->left;
		}

		const std::vector<int>& getRight() const{
			return right;
		}

		int getDotPosition() const {
			return dotPos;
		}

		int getDotSymbol() {
			if(dotPos >= right.size()) {
				return UNKNOWN_SYMBOL;
			}
			return right.at(dotPos);
		}

		bool operator== (const Production& p) const {
			return operator== (std::move(p));
		}

		bool operator== (const Production&& p) const {
			if(this->left != p.getLeft()) {
				return false;
			}

			/* c++11 specification for sequence container */
			if(this->right == p.getRight()) {

			}
			else
			{
				return false;
			}

			if(this->lookAhead == p.lookAhead) {
				
			}
			else
			{
				return false;
			}

			if(this->dotPos != p.getDotPosition()) {
				return false;
			}
			return true;
		}

		bool isSimilar(const Production& p) const {
			if(this->left != p.getLeft()) {
				return false;
			}

			/* c++11 specification for sequence container */
			if(this->right == p.getRight()) {

			}
			else
			{
				return false;
			}

			if(this->dotPos != p.getDotPosition()) {
				return false;
			}
			return true;
		}
	};

	class ProductionManager
	{
	private:
		std::map<int, std::vector<Production>> productionMap;
	public:
		ProductionManager() = default;
		~ProductionManager() = default;
		void initProductions(std::map<int, Symbols>& symp) {
			if(productionMap.size() > 0) {
				return;
			}
			int size = productionMap.size();
			int left;
			auto it = symp.begin();
			std::vector<std::vector<int>>::iterator jt;
			for (; it != symp.end(); it++)
			{
				left = it->first;
				jt = it->second.productions.begin();
				std::vector<Production> tmp;
				for (; jt!= it->second.productions.end(); jt++)
				{
					tmp.push_back(Production(left, 0, *jt, {}));
				}
				
				productionMap.insert({left, tmp});
			}
		}
		
		/* return rvalue we use stack instead of heap */
		std::vector<Production> getProduction(int left) {
			try {
				return productionMap.at(left);
			}
			catch(...)
			{
				return {};
			}
		}
	};
}
