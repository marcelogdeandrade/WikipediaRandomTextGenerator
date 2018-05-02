#include "tree.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;

void Tree::create_tree(ngram_map ngrams, int size_ngram){
	Node* root = new Node();
	root->word.assign("$");
	this->root = root;

	std::map<int, std::string> cur_string;
	std::map<int, Node*> cur_node;
	cur_node[0] = root;
	for (ngram_map::iterator it = ngrams.begin(); it != ngrams.end(); it++ ) {
		for (int i = 0; i < size_ngram; i++){
			if (cur_string[i].compare(it->first[i]) != 0){
				cur_string[i].assign(it->first[i]);
				Node* node = new Node();
				node->word.assign(cur_string[i]);
				node->level = i;
				cur_node[i]->children[cur_string[i]] = node;
				if (i == size_ngram - 1){
					node->count = it->second;
				} else {
					cur_node[i + 1] = node;
					for (int j = i + 1; j < size_ngram; j++){
						cur_string[j].assign("");
					}
				}
			}
		}
	}
	this->calc_node_counts(this->root);
}

void Tree::print_tree(Node node){
	for (int i = 0; i < node.level; i++){
		std::cout << "    ";
	}
	std::cout << node.word << ": " << node.count << "\n";
	for (std::map<std::string, Node*>::iterator it = node.children.begin(); it != node.children.end(); it++ )
	{	
		this->print_tree(*it->second);
	}
}

int Tree::calc_node_counts(Node* node){
	if (node->children.size() != 0){
		int sum = 0;
		for (std::map<std::string, Node*>::iterator it = node->children.begin(); it != node->children.end(); it++ )
		{	
			sum += this->calc_node_counts(it->second);
		}
		node->count = sum;
	} 
	return node->count;
}
