#include <Arduino.h>
#include <PuzzleModule.h>
#include <Rules.h>
#include <utils/Button.h>

#include "display.h"

const int redPin = 27, greenPin = 14;
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
Button submitButton;

struct SelectButtonFunction {
    int col, row;
    SelectButtonFunction(int col, int row): col(col), row(row) {}
    void operator() (State state) {
        if (state != State::PRESSED) return;
        int offset = (row == 0) ? -1 : 1;
        position[col].selected = (position[col].selected + offset + OPTIONS) % OPTIONS;
        Display::word[col] = position[col].letters[position[col].selected];
    }
};

int answer;

bool valid_options() {
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
        if (found_word) {
            return false;
        }
    }
    return true;
}

void generate_options() {
    answer = esp_random() % (words.size());
    for (int i = 0; i < WORD_LENGTH; i++) {
        position[i].letters[0] = words[answer][i];
        for (int j = 1; j < OPTIONS; j++)
            position[i].letters[j] = (esp_random() % LETTERS) + 'a';
    }
    if (!valid_options()) {
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
    for (int i = 0; i < WORD_LENGTH; i++) {
        Display::word[i] = position[i].letters[position[i].selected];
    }
}

void onSeed(int seed) {
    words = generate_words(seed);
    generate_options();
}

void submit(State state) {
    if (state != State::PRESSED) return;
    bool correct = true;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (position[i].letters[position[i].selected] != words[answer][i]) {
            correct = false;
            break;
        }
    }
    if (correct) {
        PuzzleModule::solve();
    } else {
        PuzzleModule::strike();
    }
}

void setup() {
    pinMode(CLOCK, OUTPUT);
    pinMode(STORE, OUTPUT);
    pinMode(SER, OUTPUT);
    pinMode(SUBMIT_BUTTON_PIN, INPUT);

    for (int i = 0; i < DIGITS; i++) {
        pinMode(DIGIT_PINS[i], OUTPUT);
        digitalWrite(DIGIT_PINS[i], LOW);
    }

    PuzzleModule::StatusLight statusLight;
    statusLight.init(redPin, greenPin);

    if (!PuzzleModule::init(statusLight, onSeed))
        ESP.restart();

    for (int i = 0; i < BUTTON_COLS; i++) {
        pinMode(BUTTON_COL_PINS[i], INPUT);
    }
    for (int i = 0; i < BUTTON_ROWS; i++) {
        pinMode(BUTTON_ROW_PINS[i], OUTPUT);
        digitalWrite(BUTTON_ROW_PINS[i], LOW);
    }

    for (int i = 0; i < BUTTON_COLS; i++) {
        for (int j = 0; j < BUTTON_ROWS; j++) {
            buttons[i][j] = Button(BUTTON_COL_PINS[i]);
            buttons[i][j].onStateChange = SelectButtonFunction(i, j);
        }
    }

    submitButton = Button(SUBMIT_BUTTON_PIN);
    submitButton.onStateChange = submit;

    Display::init();
}

void loop() {
  PuzzleModule::update();
  if (words.size() == 0) return;
  Display::update();
  for (int i = 0; i < BUTTON_ROWS; i++) {
    digitalWrite(BUTTON_ROW_PINS[(i - 1 + BUTTON_ROWS) % BUTTON_ROWS], LOW);
    digitalWrite(BUTTON_ROW_PINS[i], HIGH);
    for (int j = 0; j < BUTTON_COLS; j++) {
      buttons[j][i].update();
    }
  }
  submitButton.update();
}