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
    if(data.charAt(0) == 'f'){
        data.remove(0);
        effect = true;
        effectData = JSON.parse(data);
        startTime = millis();
        for (size_t i = 0; i < effectData["la"].length; i++){
            startTime[i] = millis();
        }
    }
    else{
        effect = false;
        effectData = JSON.parse(data);
        uint8_t color[] = HsvToRgb(effectData); 
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};

        red = (color[0] * effectBrightness) / 100;
        green = (color[1] * effectBrightness) / 100;
        blue = (color[2] * effectBrightness) / 100;

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

uint8_t* EffectEngine::HsvToRgb(JSONVar data){
    uint8_t red, green, blue;
    double hue, saturation, value;

    hue = (int)data["h"];
    saturation = (int)data["s"];
    value = (int)data["v"];

    hue = hue / 3600;
    saturation = saturation / 1000;
    value = value / 1000;

    ColorConverter::HsvToRgb(hue, saturation, value, red, green, blue);
    return {red, green, blue};
}

uint8_t** EffectEngine::buildPattern(JSONVar data){
    uint8_t returnData[ledCount][3] = {};

    for (size_t i = 0; i < data.length; i++){
        if(data[i]["ty"] == "oc"){

        }
                
        checkTime += (long)effectData["la"][i][e]["du"];
        checkTime += (long)effectData["la"][i][e]["de"];
    }

    return returnData;
}

void EffectEngine::tick(){
    if(effect){
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};

        for (size_t i = 0; i < effectData["la"].length; i++){
            long checkTime = 0;
            for (size_t e = 0; e < effectData["la"][i].length; e++){
                
                checkTime += (long)effectData["la"][i][e]["du"];
                checkTime += (long)effectData["la"][i][e]["de"];
            }
        }




        if(updateFunctionRGB) updateFunctionRGB(redarray,greenarray,bluearray);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}