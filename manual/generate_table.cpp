#include <algorithm>
#include <iostream>
#include <manual.h>
#include <stdio.h>
#include <stdlib.h>

const int COLUMNS = 5;

void generateTable(WordList &words) {
  for (int i = 0; i < WORD_COUNT / COLUMNS; i++) {
    printf("<tr>");
    for (int j = 0; j < COLUMNS; j++)
      printf("<th style=\"padding: 0.75em 1em;font-size: 120%%;font-style: "
             "normal;font-weight: 400;\">%s</th>",
             words[i * COLUMNS + j].c_str());
    printf("</tr>");
  }
}

int main(int argc, char **argv) {
  if (WORD_COUNT % COLUMNS != 0)
    throw std::runtime_error("WORD_COUNT must be divisible by COLUMNS");
  freopen("./manual/table.html", "w", stdout);
  int seed = atoi(argv[1]);
  WordList words = generateWords(seed);
  std::sort(words.begin(), words.end());
  generateTable(words);
}