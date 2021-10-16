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
            Serial.println("effect");
            Serial.println(data);
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

void EffectEngine::buildPattern(JSONVar data, uint8_t &reddata[], uint8_t &greendata[], uint8_t &bluedata[]){
    for (size_t i = 0; i < data.length(); i++){
        if(String((const char*)data[i]["ty"]) == "oc"){
            uint8_t red, green, blue;
            HsvToRgb(data[i]["co"], red, green, blue);
            int start = map((int)data[i]["st"], 0, 1000, 0, ledCount);
            int count = map((int)data[i]["en"], 0, 1000, 0, ledCount) - start;
            for (size_t e = 0; e <= count; e++){
                reddata[e + start][0] = red;
                greendata[e + start][1] = green;
                bluedata[e + start][2] = blue;
            }
        }
    }
}

JSONVar EffectEngine::mergePattern(JSONVar data1, JSONVar data2){
    for (size_t i = 0; i < ledCount; i++){
        if((int)data2[i][0] != NULL){
            data1[i][0] = (int)data2[i][0];
            data1[i][1] = (int)data2[i][1];
            data1[i][2] = (int)data2[i][2];
        }
    }

    return data1;
}

void EffectEngine::buildLayer(JSONVar data, int index, uint8_t &reddata[], uint8_t &greendata[], uint8_t &bluedata[]){
    long procTime = millis() - startTime[index];
    long checkTime = 0;

    for (size_t i = 0; i < data.length(); i++){
        if(checkTime < procTime){
            if(String((const char*)data[i]["ty"]) == "st"){
                uint8_t reddatap[ledCount] = {};
                uint8_t greendatap[ledCount] = {};
                uint8_t bluedatap[ledCount] = {};
                buildPattern(data[i]["da"], reddatap, greendatap, bluedatap);

                reddata = reddatap;
                greendata = greendatap;
                bluedata = bluedatap;
                //Serial.println(JSON.stringify(pattern));
                //returnData = mergePattern(returnData, pattern);
            }
        }
        checkTime += (long)data[i]["du"];
        checkTime += (long)data[i]["de"];
        if(i == data.length() - 1 && procTime > checkTime){
            startTime[index] = millis();
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
            long procTime = millis() - startTime[index];
            long checkTime = 0;

            for (size_t i = 0; i < effectData["la"][l].length(); i++){
                if(checkTime < procTime){
                    if(String((const char*)effectData["la"][l][i]["ty"]) == "st"){

                        //Start build Pattern
                            for (size_t p = 0; p < effectData["la"][l][i]["da"].length(); p++){
                                if(String((const char*)effectData["la"][l][i]["da"][p]["ty"]) == "oc"){
                                    uint8_t red, green, blue;
                                    HsvToRgb(effectData["la"][l][i]["da"][p]["co"], red, green, blue);
                                    int start = map((int)effectData["la"][l][i]["da"][p]["st"], 0, 1000, 0, ledCount);
                                    for (size_t e = 0; e <= map((int)effectData["la"][l][i]["da"][p]["en"], 0, 1000, 0, ledCount) - start; e++){
                                        reddata[e + start][0] = red;
                                        greendata[e + start][1] = green;
                                        bluedata[e + start][2] = blue;
                                    }
                                }
                            }
                        //End Build Pattern
                        
                        buildPattern(effectData["la"][l][i]["da"], reddatap, greendatap, bluedatap);
                    }
                }
                checkTime += (long)effectData["la"][l][i]["du"];
                checkTime += (long)effectData["la"][l][i]["de"];
                if(i == data.length() - 1 && procTime > checkTime){
                    startTime[index] = millis();
                }
            }
            //returnData = mergePattern(returnData, layer);
            //build LayverEnd

        }

        if(updateFunctionRGB) updateFunctionRGB(reddata,greendata,bluedata);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}