#include "ThresholdController.h"
#include <algorithm>
#include <cmath>

ThresholdController::ThresholdController(PinName rotaryPin, PinName buttonPin, float minTemp,
                                         float maxTemp)
    : _rotary(rotaryPin), _button(buttonPin, PullDown), _minTemp(minTemp), _maxTemp(maxTemp),
      _lowThreshold(18.0f), _highThreshold(28.0f), _isEditingHigh(false)
{
}

void ThresholdController::update()
{
    float normalized = _rotary.read(); // Zakres 0.0 - 1.0
    float mappedTemp = std::round((_minTemp + normalized * (_maxTemp - _minTemp)) * 2.0f) / 2.0f;

    _isEditingHigh = (_button.read() == 1);

    if (!_isEditingHigh)
    {
        _lowThreshold = std::min(mappedTemp, _highThreshold - 2.0f);
    }
    else
    {
        _highThreshold = std::max(mappedTemp, _lowThreshold + 2.0f);
    }
}

float ThresholdController::getLowThreshold() const
{
    return _lowThreshold;
}

float ThresholdController::getHighThreshold() const
{
    return _highThreshold;
}

bool ThresholdController::isEditingHigh() const
{
    return _isEditingHigh;
}