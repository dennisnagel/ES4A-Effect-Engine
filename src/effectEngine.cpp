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
        for (size_t i = 0; i < effectData["la"].length(); i++){
            startTime[i] = millis();
        }
    }
    else{
        effect = false;
        effectData = JSON.parse(data);
        uint8_t color[3] = HsvToRgb(effectData); 
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};

        color[0] = (color[0] * effectBrightness) / 100;
        color[1] = (color[1] * effectBrightness) / 100;
        color[2] = (color[2] * effectBrightness) / 100;

        for (size_t i = 0; i < ledCount; i++){
            redarray[i] = color[0];
            greenarray[i] = color[1];
            bluearray[i] = color[2];
        }
        if(updateFunctionRGB) updateFunctionRGB(redarray,greenarray,bluearray);
    }
}

void EffectEngine::setBrightness(int brightness){
    effectBrightness = brightness;
}

void EffectEngine::HsvToRgb(JSONVar data, uint8_t &returnData[]){
    uint8_t red, green, blue;
    double hue, saturation, value;

    hue = (int)data["h"];
    saturation = (int)data["s"];
    value = (int)data["v"];

    hue = hue / 3600;
    saturation = saturation / 1000;
    value = value / 1000;

    ColorConverter::HsvToRgb(hue, saturation, value, red, green, blue);
    returnData[0] = red;
    returnData[1] = green;
    returnData[2] = blue;
}

void EffectEngine::buildPattern(JSONVar data, uint8_t &returnData[][]){
    //uint8_t returnData[ledCount][3] = {};

    for (size_t i = 0; i < data.length(); i++){
        if(data[i]["ty"] == "oc"){

        }
    }

    return returnData;
}

void EffectEngine::tick(){
    if(effect){
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};

        for (size_t i = 0; i < effectData["la"].length(); i++){
           // long checkTime = 0;
            for (size_t e = 0; e < effectData["la"][i].length(); e++){
                
                //checkTime += (long)effectData["la"][i][e]["du"];
               // checkTime += (long)effectData["la"][i][e]["de"];
            }
        }




        if(updateFunctionRGB) updateFunctionRGB(redarray,greenarray,bluearray);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}