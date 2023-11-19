#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

const uint8_t EMPTY = 0b11111111;
const uint8_t A = 0b11110111;
const uint8_t B = 0b11111011;
const uint8_t C = 0b11101111;
const uint8_t D = 0b10111111;
const uint8_t E = 0b11111101;
const uint8_t F = 0b11111110;
const uint8_t G = 0b11011111;

const int CLOCK = 23, STORE = 22, SER = 21;
const int DIGITS = 10;
const int DIGIT_PINS[DIGITS] = {19, 18, 5, 17, 16, 4, 2, 15, 12, 13};

const int WORD_SIZE = 5;
const int LETTERS = 26;
const int LETTER_SEGMENTS[LETTERS + 1][2] = {
    {A & B & C & D, B &C &D}, // A
    {A & E, A &B & C & D &E}, // B
    {A & B & C, A},           // C
    {A & E, A &B & C &E},     // D
    {A & B & C & D, A},       // E
    {A & B & C & D, EMPTY},   // F
    {A & B & C, A &C &D},     // G
    {B & C & D, B &C &D},     // H
    {A & E, A &E},            // I
    {C, A &B &C},             // J
    {B & C & D, F &G},        // K
    {B & C, A},               // L
    {B & C & F, B &C &F},     // M
    {B & C & F, B &C &G},     // N
    {A & B & C, A &B &C},     // O
    {A & B & C & D, B &D},    // P
    {A & B & C, A &B & C &G}, // Q
    {A & B & C & D, B &D &G}, // R
    {A & B & D, A &C &D},     // S
    {A & E, E},               // T
    {B & C, A &B &C},         // U
    {B & C & G, F},           // V
    {B & C & G, B &C &G},     // W
    {F & G, F &G},            // X
    {F, E &F},                // Y
    {A & G, A &F},            // Z
    {EMPTY, EMPTY}            // {
};

const int DELAY = 1;

namespace Display {
char word[WORD_SIZE + 1];

void send(int8_t data) {
  shiftOut(SER, CLOCK, LSBFIRST, data);
  digitalWrite(STORE, HIGH);
  digitalWrite(STORE, LOW);
}

void setup() {
  for (int i = 0; i < WORD_SIZE; i++)
    word[i] = '{';
  pinMode(CLOCK, OUTPUT);
  pinMode(STORE, OUTPUT);
  pinMode(SER, OUTPUT);
  for (int i = 0; i < DIGITS; i++) {
    pinMode(DIGIT_PINS[i], OUTPUT);
    digitalWrite(DIGIT_PINS[i], LOW);
  }
}

void update() {
  int index = (millis() / DELAY) % DIGITS;
  int position = index / 2;
  int part = index % 2;
  int lastIndex = (index - 1 + DIGITS) % DIGITS;
  uint8_t segments = LETTER_SEGMENTS[word[position] - 'a'][part];
  digitalWrite(DIGIT_PINS[lastIndex], LOW);
  send(segments);
  digitalWrite(DIGIT_PINS[index], HIGH);
}
}; // namespace Display

#endif // DISPLAY_H