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
        data.remove(0, 1);
        JSONVar newData = JSON.parse(data);
        effect = true;
        if(JSON.stringify(newData) != JSON.stringify(effectData)){
            effectData = newData;
            startTime[effectData["la"].length()] = {}; 
            for (size_t i = 0; i < effectData["la"].length(); i++){
                startTime[i] = millis();
            }
        }
    }
    else{
        JSONVar newData = JSON.parse(data);
        effect = false;
        if(JSON.stringify(newData) != JSON.stringify(effectData)){
            effectData = newData;
            uint8_t color[3] = {0, 0, 0};
            HsvToRgb(effectData, color[0], color[1], color[2]); 
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
            if(updateFunctionRGB) updateFunctionRGB(redarray, greenarray, bluearray);
        }
    }
}

void EffectEngine::setBrightness(int brightness){
    effectBrightness = brightness;
}

void EffectEngine::HsvToRgb(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b){
    uint8_t red, green, blue;
    double hue, saturation, value;

    hue = (int)data["h"];
    saturation = (int)data["s"];
    value = (int)data["v"];

    hue = hue / 3600;
    saturation = saturation / 1000;
    value = value / 1000;

    ColorConverter::HsvToRgb(hue, saturation, value, red, green, blue);
    r = red;
    g = green;
    b = blue;
}

void EffectEngine::buildPattern(JSONVar data, uint8_t reddata[], uint8_t greendata[], uint8_t bluedata[]){
    for (size_t p = 0; p < data.length(); p++){
        if(String((const char*)data[p]["ty"]) == "oc"){
            uint8_t red, green, blue;
            HsvToRgb(data[p]["co"], red, green, blue);
            int start = map((int)data[p]["st"], 0, 1000, 0, ledCount);
            for (size_t e = 0; e < map((int)data[p]["en"], 0, 1000, 0, ledCount) - start; e++){
                reddata[e + start] = red; //intergate brightness
                greendata[e + start] = green;
                bluedata[e + start] = blue;
            }
        }
    }
}

void EffectEngine::tick(){
    if(effect){
        uint8_t reddata[ledCount] = {};
        uint8_t greendata[ledCount] = {};
        uint8_t bluedata[ledCount] = {};

        for (size_t i = 0; i < ledCount; i++){
            reddata[i] = 0;
            greendata[i] = 0;
            bluedata[i] = 0;
        }

        for (size_t l = 0; l < effectData["la"].length(); l++){
           //build Layver Start
            long procTime = millis() - startTime[l];
            long checkTime = 0;

            for (size_t i = 0; i < effectData["la"][l].length(); i++){
                if(checkTime < procTime){
                    if(String((const char*)effectData["la"][l][i]["ty"]) == "st"){
                        //Start build Pattern
                        uint8_t reddatap[ledCount] = {};
                        uint8_t greendatap[ledCount] = {};
                        uint8_t bluedatap[ledCount] = {};
                        for (size_t e = 0; e < ledCount; e++){
                            reddata[i] = NULL;
                            greendata[i] = NULL;
                            bluedata[i] = NULL;
                        }
                        buildPattern(effectData["la"][l][i]["da"], reddatap, greendatap, bluedatap);
                        for (size_t e = 0; e < ledCount; e++){
                            if(reddatap[e] >= 0){
                                reddata[i] = reddatap[e];
                                greendata[i] = greendatap[e];
                                bluedata[i] = bluedatap[e];
                            }
                        }
                        //End Build Pattern
                        
                    }
                }
                checkTime += (long)effectData["la"][l][i]["du"];
                checkTime += (long)effectData["la"][l][i]["de"];
                if(i == effectData["la"][l].length() - 1 && procTime > checkTime){
                    startTime[l] = millis();
                }
            }
            //build LayverEnd

        }

        if(updateFunctionRGB) updateFunctionRGB(reddata,greendata,bluedata);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}