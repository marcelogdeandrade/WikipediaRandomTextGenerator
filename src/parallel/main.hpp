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
#endif