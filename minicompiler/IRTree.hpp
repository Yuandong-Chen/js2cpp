#pragma once
#include "../miniparser/AST.hpp"
// shrink the tree size for those who only has only 1 child

namespace miniir {
	std::shared_ptr<Node> shrinkASTTree(std::shared_ptr<Node> node) {
		// if(node->branch.size() > 0) {
		// 	node->branch.clear();
		// }
		
		// return;

		if(node->branch.size() == 0) {
			return node;
		}

		if(node->branch.size() > 1) {
			for (int i = 0; i < node->branch.size(); ++i)
			{
				node->branch[i] = shrinkASTTree(node->branch[i]);
			}
			return node;	
		}

		if(node->kind == math_e || node->kind == bool_e || node->kind == rvalue_l || node->kind == lvalue_l) {
			return shrinkASTTree(node->branch[0]);
		}

		node->branch[0] = shrinkASTTree(node->branch[0]);
		return node;
	}

	class IRTree
	{
	public:
		std::shared_ptr<Node> irtree;
		IRTree() {
			miniparser::AST ast;
			irtree = ast.astRoot;
			irtree = miniir::shrinkASTTree(irtree);
		}
		~IRTree() = default;
		std::shared_ptr<Node> getIRTree() {
			return irtree;
		}
	};
}


// int main(int argc, char const *argv[])
// {
//     miniir::IRTree irtree;
//     miniparser::AST::printASTTree(irtree.getIRTree());
// 	return 0;
// }