#ifndef READ_INPUT_H
#define READ_INPUT_H

#include "pugixml.hpp"
#include <boost/tokenizer.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

std::vector<std::string> read_xml_input(std::string &file_name);

std::vector<std::string> tokenize_text(std::string& text);

void clean_text(std::string& text);

bool remove_special_characters(char c);

bool is_punct(char c);

#endif