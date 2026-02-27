#include <manual.h>
#include <rules.h>

#include <algorithm>

const int COLUMNS = 5;

manual::json generate_json_for_code(uint16_t code) {
  if (WORD_COUNT % COLUMNS != 0) throw std::runtime_error("WORD_COUNT must be divisible by COLUMNS");

  WordList words = generate_words(code);
  std::sort(words.begin(), words.end());

  manual::json data = manual::init(MODULE_NAME, MODULE_NAME,
                                   "Fortunately this password doesn't seem to meet standard government "
                                   "security requirements: 22 characters, mixed case, numbers in random "
                                   "order without any palindromes above length 3.",
                                   APP_VERSION);

  data["rows"] = manual::json::array();
  for (int i = 0; i < WORD_COUNT / COLUMNS; i++) {
    manual::json row = manual::json::array();
    for (int j = 0; j < COLUMNS; j++) row.push_back(words[i * COLUMNS + j]);
    data["rows"].push_back(row);
  }
  return data;
}

int main(int argc, char** argv) { manual::run(argc, argv, generate_json_for_code); }