#include <Arduino.h>
#include "EffectEngine.h"
#include <Arduino_JSON.h>

EffectEngine::EffectEngine(){
}

EffectEngine::EffectEngine(int leds){
}

void EffectEngine::setData(String data){
    effectData = data;
    //TODO check if effect
}

void EffectEngine::setBrightness(int brightness){
    effectBrightness = brightness;
}

void EffectEngine::tick(){
    
}

void EffectEngine::setListener(callbackFunction function){
    updateFunction = function;
}