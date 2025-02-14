#include <algorithm>
#include <manual.h>
#include <rules.h>

const int COLUMNS = 5;

int main(int argc, char **argv) {
  if (WORD_COUNT % COLUMNS != 0)
    throw std::runtime_error("WORD_COUNT must be divisible by COLUMNS");

  int code = atoi(argv[1]);
  WordList words = generateWords(code);
  std::sort(words.begin(), words.end());

  manual::json data = manual::init(
      "Passwords", "Passwords",
      "Fortunately this password doesn't seem to meet standard government "
      "security requirements: 22 characters, mixed case, numbers in random "
      "order without any palindromes above length 3.",
      APP_VERSION);

  data["rows"] = manual::json::array();

  for (int i = 0; i < WORD_COUNT / COLUMNS; i++) {
    manual::json row = manual::json::array();
    for (int j = 0; j < COLUMNS; j++)
      row.push_back(words[i * COLUMNS + j]);
    data["rows"].push_back(row);
  }

  manual::save(data, code);
}