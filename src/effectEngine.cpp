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

JSONVar EffectEngine::buildPattern(JSONVar data){
    JSONVar returnData = JSON.parse("[]");

    for (size_t i = 0; i < ledCount; i++){
        returnData[i] = JSON.parse("[]");
        returnData[i][0] = NULL;
        returnData[i][1] = NULL;
        returnData[i][2] = NULL;
    }

    for (size_t i = 0; i < data.length(); i++){
        if(String((const char*)data[i]["ty"]) == "oc"){
            uint8_t red, green, blue;
            HsvToRgb(data[i]["co"], red, green, blue);
            int start = map((int)data[i]["st"], 0, 1000, 0, ledCount);
            int end = map((int)data[i]["en"], 0, 1000, 0, ledCount);
            int count = end - start;
            for (size_t e = 0; e <= count; e++){
                returnData[e + start][0] = red;
                returnData[e + start][1] = green;
                returnData[e + start][2] = blue;
            }
        }
    }

    return returnData;
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

JSONVar EffectEngine::buildLayer(JSONVar data, int index){
    JSONVar returnData = JSON.parse("[]");

    for (size_t i = 0; i < ledCount; i++){
        returnData[i] = JSON.parse("[]");
        returnData[i][0] = NULL;
        returnData[i][1] = NULL;
        returnData[i][2] = NULL;
    }

    long procTime = millis() - startTime[index];
    long checkTime = 0;

    for (size_t i = 0; i < data.length(); i++){
        if(checkTime < procTime){
            if(String((const char*)data[i]["ty"]) == "st"){
                JSONVar pattern = buildPattern(data[i]["da"]);
                Serial.println(JSON.stringify(pattern));
                returnData = mergePattern(returnData, pattern);
            }
        }
        checkTime += (long)data[i]["du"];
        checkTime += (long)data[i]["de"];
        if(i == data.length() - 1 && procTime > checkTime){
            startTime[index] = millis();
        }
    }

    return returnData;
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

        /*for (size_t i = 0; i < effectData["la"].length(); i++){
            JSONVar layer = buildLayer(effectData["la"][i], i);
            //Serial.println(JSON.stringify(layer));
            returnData = mergePattern(returnData, layer);
        }*/

        for (size_t i = 0; i < ledCount; i++){
            reddata[i] = (int)returnData[i][0];
            greendata[i] = (int)returnData[i][1];
            bluedata[i] = (int)returnData[i][2];
        }

        if(updateFunctionRGB) updateFunctionRGB(reddata,greendata,bluedata);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}