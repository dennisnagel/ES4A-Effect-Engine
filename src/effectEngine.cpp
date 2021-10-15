#include <Arduino.h>
#include "EffectEngine.h"
#include <Arduino_JSON.h>
#include "ColorConverterLib.h"

EffectEngine::EffectEngine(){
}

EffectEngine::EffectEngine(int leds){
    ledCount = leds;
}

void EffectEngine::setData(String data){
    if(data.charAt(0) == "f"){
        data.remove(0);
        effect = true;
        effectData = JSON.parse(data);
    }
    else{
        effect = false;
        effectData = JSON.parse(data);
        if(updateFunctionRGB) updateFunctionRGB({0},{0},{0});
        uint8_t red, green, blue;
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};
        double hue, saturation, value;

        hue = (int)effectData["h"];
        saturation = (int)effectData["s"];
        value = (int)effectData["v"];

        hue = hue / 3600;
        saturation = saturation / 1000;
        value = value / 1000;

        ColorConverter::HsvToRgb(hue, saturation, value, red, green, blue);

        red = (red * effectBrightness) / 100;
        green = (green * effectBrightness) / 100;
        blue = (blue * effectBrightness) / 100;

        for (size_t i = 0; i < ledCount; i++){
            redarray[i] = red;
            greenarray[i] = green;
            bluearray[i] = blue;
        }
        if(updateFunctionRGB) updateFunctionRGB(redarray,greenarray,bluearray);
    }
}

void EffectEngine::setBrightness(int brightness){
    effectBrightness = brightness;
}

void EffectEngine::tick(){
    if(effect){

    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}