#include "ThresholdController.h"

ThresholdController::ThresholdController(PinName rotaryPin, PinName buttonPin, float minTemp, float maxTemp)
    : _rotary(rotaryPin),
      _button(buttonPin),
      _minTemp(minTemp),
      _maxTemp(maxTemp),
      _lowThreshold(18.0f),
      _highThreshold(28.0f),
      _isEditingHigh(false) {
}

void ThresholdController::update() {
    float normalized = _rotary.read(); // Zakres 0.0 - 1.0
    float mappedTemp = _minTemp + normalized * (_maxTemp - _minTemp);

    _isEditingHigh = (_button.read() == 1);

    if (!_isEditingHigh) {
        _lowThreshold = mappedTemp;
    } 
    else {
        _highThreshold = mappedTemp;
    }
}

float ThresholdController::getLowThreshold() const {
    return _lowThreshold;
}

float ThresholdController::getHighThreshold() const {
    return _highThreshold;
}

bool ThresholdController::isEditingHigh() const {
    return _isEditingHigh;
}