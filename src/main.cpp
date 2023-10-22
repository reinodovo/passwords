#include <Arduino.h>
#include <PuzzleModule.h>
#include <Rules.h>

#include "display.h"
#include "button.h"

const int redPin = 27, greenPin = 14;
const int SEGMENTS = 7;

WordList words;
const int OPTIONS = 6;
struct PositionOptions {
    char letters[OPTIONS];
    int selected;
} position[WORD_LENGTH];

Button buttons[COLS * ROWS];

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
        Display::word[i] = position[i].letters[position[i].selected];
    }
    // TODO: random shuffle
}

void button_pressed(ButtonState state, int col, int row) {
    if (state != ButtonState::Pressed) return;
    int offset = (row == 0) ? -1 : 1;
    position[col].selected = (position[col].selected + offset + OPTIONS) % OPTIONS;
    Display::word[col] = position[col].letters[position[col].selected];
}

void setup() {
    pinMode(CLOCK, OUTPUT);
    pinMode(STORE, OUTPUT);
    pinMode(SER, OUTPUT);

    for (int i = 0; i < DIGITS; i++) {
        pinMode(DIGIT_PINS[i], OUTPUT);
        digitalWrite(DIGIT_PINS[i], LOW);
    }

    PuzzleModule::StatusLight statusLight;
    statusLight.init(redPin, greenPin);

    if (!PuzzleModule::init(statusLight))
    {
        ESP.restart();
    }

    for (int i = 0; i < COLS; i++) {
        pinMode(COL_PINS[i], INPUT);
    }
    for (int i = 0; i < ROWS; i++) {
        pinMode(ROW_PINS[i], OUTPUT);
        digitalWrite(ROW_PINS[i], LOW);
    }
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            buttons[i * ROWS + j].init(i, j, button_pressed);
        }
    }

    Display::init();
}

unsigned long lastAttempt = 0;
unsigned long attemptGap = 100;

void loop() {
  PuzzleModule::update();
  if (words.size() == 0 && millis() - lastAttempt > attemptGap) {
    lastAttempt = millis();
    PuzzleModule::withBombInfo([](BombInfo info) {
      words = generate_words(info.seed);
      generate_options();
    });
    return;
  }
  Display::update();
  for (int i = 0; i < ROWS * COLS; i++) {
    buttons[i].update();
  }
}