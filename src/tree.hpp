#include <iostream>
#include <string>
#include <vector>
#include <map>

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;

struct Node {
	std::map<std::string, Node*> children; 
	int count = 0;
	int level = 0;
	std::string word = "";
};

class Tree {
public:
	Node* root;
	void create_tree(ngram_map ngrams, int size_ngram);
	void print_tree(Node node);
private:
	int calc_node_counts(Node* node);	
};