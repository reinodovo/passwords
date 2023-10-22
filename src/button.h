#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include <Arduino.h>

const int LONG_PRESS_DURATION = 1000;
const int DEBOUNCE_DELAY = 50;

const int COLS = 5;
const int COL_PINS[COLS] = {34, 39, 36, 35, 32};
const int ROWS = 2;
const int ROW_PINS[ROWS] = {33, 25};

enum class ButtonState {
    Pressed,
    LongPress,
    Released
};

using Callback = std::function<void(ButtonState, int, int)>;

struct Button {
    int _colPin, _rowPin;
    int _col, _row;
    ButtonState _state;
    unsigned long _lastPress, _lastDebounce;
    Callback _callback;
    void init(int col, int row, Callback callback) {
        _col = col;
        _row = row;
        _colPin = COL_PINS[col];
        _rowPin = ROW_PINS[row];
        _lastPress = 0;
        _state = ButtonState::Released;
        _callback = callback;
    }
    void update() {
        ButtonState nextState = state();
        if (nextState != _state) {
            _lastDebounce = millis();
        }
        if (millis() - _lastDebounce < DEBOUNCE_DELAY) {
            if (nextState != _state) {
                _callback(nextState, _col, _row);
            }
        }
        _state = nextState;
    }
    ButtonState state() {
        digitalWrite(_rowPin, HIGH);
        bool res = digitalRead(_colPin);
        digitalWrite(_rowPin, LOW);
        if (res == HIGH) {
            if (_lastPress == 0) {
                _lastPress = millis();
                return ButtonState::Pressed;
            }
            else if (millis() - _lastPress > LONG_PRESS_DURATION) {
                return ButtonState::LongPress;
            }
        }
        else {
            if (_lastPress != 0) {
                _lastPress = 0;
                return ButtonState::Released;
            }
        }
        return ButtonState::Released;
    }
};

#endif // BUTTON_H