#include "read_input.hpp"
#include "main.hpp"
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

namespace mpi = boost::mpi;


#define NGRAM_SIZE 3
#define SIZE_TEXT 300
#define TAG 42

#define MASTER 0

typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;



int main(){

	mpi::environment env;
	mpi::communicator world;

	mpi::timer timer;

    if (world.rank() != MASTER){
	    std::string file_name = "files_mini_small/wikipedia-mini-small_" + std::to_string(world.rank()) + ".xml" ;
	    std::cout << file_name << "\n";
	    std::vector<std::string> input = read_xml_input(file_name);

	    std::cout << "Leitura de arquivos: " << timer.elapsed() << "\n";
	    timer.restart();

	    std::vector<ngram> input_ngrams = doc_to_ngram(input, NGRAM_SIZE);
	   
	    std::cout << "Criar NGramas: " << timer.elapsed() << "\n";
	    timer.restart();

    	ngram_map ngrams_mapped = count_ngrams(input_ngrams);

	    std::cout << "Mapear NGramas: " << timer.elapsed() << "\n";
	    timer.restart();

    	Tree* tree = new Tree();
    	tree->create_tree(ngrams_mapped, NGRAM_SIZE);

	    std::cout << "Criar arvore: " << timer.elapsed() << "\n";
	    timer.restart();

    	std::vector<std::string> first_words;
    	std::map<std::string, int> next_word_count;
    	std::string cur_word;

    	// Primeiras N Palavras:
    	for (int i = 0; i < NGRAM_SIZE; i++){
    		next_word_count = get_count_next_word(tree, first_words, i);
    		world.send(MASTER, TAG, next_word_count);
    		world.recv(MASTER, TAG, cur_word);
    		first_words.push_back(cur_word);
    	}

    	for (int i = 0; i < SIZE_TEXT - NGRAM_SIZE; i++){
    		first_words.erase(first_words.begin());
    		next_word_count = get_count_next_word(tree, first_words, NGRAM_SIZE - 1);
    		world.send(MASTER, TAG, next_word_count);
    		world.recv(MASTER, TAG, cur_word);
    		first_words.push_back(cur_word);
    	}
    } else {
    	std::string final_text = "";
    	for (int i = 0; i < SIZE_TEXT; i++){
	    	std::vector<std::string> words;
	    	std::vector<int> probs;
	    	std::map<std::string, int> next_word_count;
	    	for (int j = 1; j < world.size(); j++){
				world.recv(j, TAG, next_word_count);
				for (std::map<std::string, int>::iterator it = next_word_count.begin(); it != next_word_count.end(); it++ ){
					auto index = std::find(words.begin(), words.end(), it->first);
					if (index != words.end()){
						auto int_index = std::distance(words.begin(), index);
						probs[int_index] += it->second;
					} else {
						words.push_back(it->first);
						probs.push_back(it->second);
					}
				}
			}
			std::random_device rd;
			std::mt19937 gen(rd());
			std::discrete_distribution<int> d(probs.begin(), probs.end());
			int word_index = d(gen);
			std::string word = words[word_index];
			final_text += " " + word;
			for (int j = 1; j < world.size(); j++){
				world.send(j, TAG, word);
			}
    	}
    	std::cout << "Final text: " << "\n";
    	std::cout << final_text << "\n";
    }
    
    return 0;
}


std::vector<ngram> doc_to_ngram(std::vector<std::string> &doc, int n){
	std::vector<ngram> doc_ngrams;
	for (unsigned int i = 0; i < doc.size() - n + 1; i ++){
		std::vector<std::string> tokens;
		for (int j = 0; j < n; j ++){
			tokens.push_back(doc[i + j]);
		}
		doc_ngrams.push_back(tokens);
	}
	return doc_ngrams;
}

std::map<std::string, int> get_count_next_word(Tree* tree, std::vector<std::string> n_words, int size_ngram){
	std::map<std::string, int> result;
	Node* node = tree->root;
	for (int i = 0; i < size_ngram; i++){
		std::string key = n_words[i];
		node = node->children[key];
	}
	for (std::map<std::string, Node*>::iterator it = node->children.begin(); it != node->children.end(); it++ ){
		result[it->first] = it->second->count;
	}
	return result;
}

// Desempenho disso está MUITO ruim
ngram_map count_ngrams(std::vector<ngram> ngrams){
	ngram_map result;
	for (unsigned int  i = 0; i < ngrams.size(); i++){
		if (result.count(ngrams[i]) == 0){
			// int num_occur = std::count(ngrams.begin(), ngrams.end(), ngrams[i]);
			result[ngrams[i]] = 1;
		} else {
			result[ngrams[i]] += 1;
		}
	}
	return result;
}
