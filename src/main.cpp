#include <Arduino.h>
#include <display.h>
#include <modules/puzzle_module.h>
#include <rules.h>
#include <utils/button.h>

const int RED_PIN = 27, GREEN_PIN = 14;
PuzzleModule module(StatusLight(RED_PIN, GREEN_PIN));

uint16_t manual_code;
bool should_generate_manual;

const int SEGMENTS = 7;
const int SUBMIT_BUTTON_PIN = 26;

WordList words;
const int OPTIONS = 6;
struct PositionOptions {
  char letters[OPTIONS];
  int selected;
} position[WORD_LENGTH];

const int BUTTON_COLS = 5;
const int BUTTON_COL_PINS[BUTTON_COLS] = {34, 39, 36, 35, 32};
const int BUTTON_ROWS = 2;
const int BUTTON_ROW_PINS[BUTTON_ROWS] = {33, 25};

Button buttons[BUTTON_COLS][BUTTON_ROWS];
Button submit_button;

struct SelectButtonFunction {
  int col, row;
  SelectButtonFunction(int col, int row) : col(col), row(row) {}
  void operator()(ButtonState state, ButtonState _) {
    if (state != ButtonState::Pressed) return;
    int offset = (row == 0) ? -1 : 1;
    position[col].selected = (position[col].selected + offset + OPTIONS) % OPTIONS;
    Display::word[col] = position[col].letters[position[col].selected];
  }
};

int answer;

bool validate_options() {
  for (int i = 0; i < words.size(); i++) {
    if (i == answer) continue;
    bool found_word = true;
    for (int j = 0; j < WORD_LENGTH; j++) {
      bool found_letter = false;
      for (int k = 0; k < OPTIONS; k++) {
        if (position[j].letters[k] == words[i][j]) {
          found_letter = true;
          break;
        }
      }
      if (!found_letter) {
        found_word = false;
        break;
      }
    }
    if (found_word) return false;
  }
  return true;
}

void generate_options() {
  answer = esp_random() % (words.size());
  for (int i = 0; i < WORD_LENGTH; i++) {
    position[i].letters[0] = words[answer][i];
    for (int j = 1; j < OPTIONS; j++) {
      int letter = (esp_random() % LETTERS) + 'a';
      bool alread_used = false;
      for (int k = 0; k < j; k++) {
        if (position[i].letters[k] == letter) {
          alread_used = true;
          break;
        }
      }
      if (alread_used) {
        j--;
        continue;
      }
      position[i].letters[j] = letter;
    }
  }
  if (!validate_options()) {
    generate_options();
    return;
  }
  for (int i = 0; i < WORD_LENGTH; i++) {
    for (int j = OPTIONS - 1; j >= 0; j--) {
      int k = esp_random() % (j + 1);
      char temp = position[i].letters[j];
      position[i].letters[j] = position[i].letters[k];
      position[i].letters[k] = temp;
    }
  }
}

void start() {
  for (int i = 0; i < WORD_LENGTH; i++) Display::word[i] = position[i].letters[position[i].selected];
}

void restart() {
  generate_options();
  words.clear();
  Display::setup();
}

void on_manual_code(int code) {
  words = generate_words(code);
  generate_options();
}

void submit(ButtonState state, ButtonState _) {
  if (state != ButtonState::Pressed) return;
  bool correct = true;
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (position[i].letters[position[i].selected] != words[answer][i]) {
      correct = false;
      break;
    }
  }
  if (correct)
    module.solve();
  else
    module.strike();
}

void setup() {
  module.on_manual_code([&](uint16_t code) {
    manual_code = code;
    should_generate_manual = true;
  });
  module.on_start(start);
  module.on_reset(restart);

  if (!module.setup()) ESP.restart();

  for (int i = 0; i < BUTTON_ROWS; i++) {
    pinMode(BUTTON_ROW_PINS[i], OUTPUT);
    digitalWrite(BUTTON_ROW_PINS[i], LOW);
  }

  for (int i = 0; i < BUTTON_COLS; i++) {
    for (int j = 0; j < BUTTON_ROWS; j++) {
      buttons[i][j] = Button(BUTTON_COL_PINS[i]);
      buttons[i][j].on_state_change(SelectButtonFunction(i, j));
    }
  }

  submit_button = Button(SUBMIT_BUTTON_PIN);
  submit_button.on_state_change(submit);

  Display::setup();
}

void loop() {
  if (should_generate_manual) {
    on_manual_code(manual_code);
    should_generate_manual = false;
  }
  module.update();
  Display::update();
  if (module.get_state() != PuzzleModuleState::Started) return;
  for (int i = 0; i < BUTTON_ROWS; i++) {
    digitalWrite(BUTTON_ROW_PINS[(i - 1 + BUTTON_ROWS) % BUTTON_ROWS], LOW);
    digitalWrite(BUTTON_ROW_PINS[i], HIGH);
    for (int j = 0; j < BUTTON_COLS; j++) buttons[j][i].update();
  }
  submit_button.update();
}