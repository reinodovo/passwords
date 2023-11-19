#ifndef RULES_H
#define RULES_H

#include <string>
#include <vector>

typedef std::vector<std::string> WordList;

const int WORD_LENGTH = 5;
const int WORD_COUNT = 35;

WordList generateWords(int);

#endif // RULES_H