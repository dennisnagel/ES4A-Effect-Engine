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
            Serial.println("patt");
            JSONVar returnData = JSON.parse("[]");
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
                returnData[i] = JSON.parse("[]");
                returnData[i][0] = color[0];
                returnData[i][1] = color[1];
                returnData[i][2] = color[2];
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

JSONVar EffectEngine::mergePattern(JSONVar data1, JSONVar data2){//TODO otimise to use less variables(proces on data1 oder data2)
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
        //Serial.print("Test1");
       // JSONVar returnData = JSON.parse("[]");

       /// for (size_t i = 0; i < ledCount; i++){
        //    returnData[i] = JSON.parse("[]");
       //     returnData[i][0] = 0;
       //     returnData[i][1] = 0;
       //     returnData[i][2] = 0;
       // }

        /*for (size_t i = 0; i < effectData["la"].length(); i++){
            JSONVar layer = buildLayer(effectData["la"][i], i);
            returnData = mergePattern(returnData, layer);
        }*/

//Serial.print("Test2");
        *uint8_t reddata[ledCount] = {};
        uint8_t greendata[ledCount] = {};
        uint8_t bluedata[ledCount] = {};

        /*for (size_t i = 0; i < ledCount; i++){
            redarray[i] = (int)returnData[i][0];
            greenarray[i] = (int)returnData[i][1];
            bluearray[i] = (int)returnData[i][2];
        }*/
        uint8_t num = 10;
        Serial.print("Test3");
        for (int i = 0; i < ledCount; i++){
            Serial.println(i);
            reddata[i] = num;
            greendata[i] = num;
            bluedata[i] = num;
        }

        Serial.print("Free Memory: ");
        Serial.println(ESP.getFreeHeap());

        if(updateFunctionRGB) updateFunctionRGB(reddata,greendata,bluedata);

        //if(updateFunctionRGB)  {updateFunctionRGB();}
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}