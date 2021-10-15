#include <Arduino.h>
#include "EffectEngine.h"
#include <Arduino_JSON.h>

EffectEngine::effectEngine(){
}

EffectEngine::effectEngine(int leds){
}

void EffectEngine::setData(String data){
    effectData = data;
    //TODO check if effect
}

void EffectEngine::setBrightness(int brightness){
    this.brightness = brightness;
}

void EffectEngine::tick(){
}

void EffectEngine::setListener(callbackFunction function){
    updateFunction = function;
}