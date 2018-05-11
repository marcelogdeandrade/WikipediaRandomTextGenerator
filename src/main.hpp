#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include "tree.hpp"


typedef std::vector<std::string> ngram;
typedef std::map<ngram, int> ngram_map;

std::vector<ngram> doc_to_ngram(std::vector<std::string> &doc, int n);

ngram_map count_ngrams(std::vector<ngram> ngrams);
	
std::string create_random_text(Tree* tree,int length, int size_ngram);

ngram_map concatenate_ngram_maps(std::vector<ngram_map> vector_ngram_map);

int get_mem_usage_map(ngram_map ngrams_mapped);

std::map<std::string, int> get_count_next_word(Tree* tree, std::vector<std::string> n_words, int size_ngram);

#endif