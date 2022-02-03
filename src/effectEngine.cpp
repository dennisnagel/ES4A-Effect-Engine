#include <Arduino.h>
#include "EffectEngine.h"
#include <Arduino_JSON.h>
#include "ColorConverterLib.h"

EffectEngine::EffectEngine(){
}

EffectEngine::EffectEngine(int leds){
    ledCount = leds;
}

EffectEngine::EffectEngine(int leds, int width){
    ledCount = leds;
    _width = width;
}

EffectEngine::setColorMode(bool r, bool g, bool b, bool ww, bool nw, bool cw, bool a){
    _rActvie = r;
	_gActvie = g;
	_bActvie = b;
	_wwActvie = ww;
	_nwActvie = nw;
	_cwActvie = cw;
	_aActvie = a;

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
    if(effectBrightness < 0){
        effectBrightness = 0;
    }
    if(effectBrightness > 100){
        effectBrightness = 100;
    }
    if(!effect){
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
                reddata[e + start] = red; 
                greendata[e + start] = green;
                bluedata[e + start] = blue;
            }
        }
        if(String((const char*)data[p]["ty"]) == "fa"){
            uint8_t red1, green1, blue1, red2, green2, blue2;
            HsvToRgb(data[p]["cs"], red1, green1, blue1);
            HsvToRgb(data[p]["ce"], red2, green2, blue2);
            int end = map((int)data[p]["en"], 0, 1000, 0, ledCount);
            int start = map((int)data[p]["st"], 0, 1000, 0, ledCount);
            int count = end - start;
            for (size_t e = 0; e < count; e++){
                reddata[e + start] = map(e, 0, count, red1, red2);
                greendata[e + start] = map(e, 0, count, green1, green2);
                bluedata[e + start] = map(e, 0, count, blue1, blue2);
            }
        }
    }
}

void EffectEngine::tick(){
    if(effect){
        if(_rActive){
            uint8_t reddata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                reddata[i] = 0;
            }
        } else{
            uint8_t reddata[0] = {};
        }
        if(_gActive){
            uint8_t greendata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                greendata[i] = 0;
            }
        } else{
            uint8_t greendata[0] = {};
        }
        if(_bActive){
            uint8_t bluedata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                bluedata[i] = 0;
            }
        } else{
            uint8_t bluedata[0] = {};
        }
        if(_wwActive){
            uint8_t warmwhitedata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                warmwhitedata[i] = 0;
            }
        } else{
            uint8_t warmwhitedata[0] = {};
        }
        if(_nwActive){
            uint8_t normalwhitedata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                normalwhitedata[i] = 0;
            }
        } else{
            uint8_t normalwhitedata[0] = {};
        }
        if(_cwActive){
            uint8_t coldwhitedata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                coldwhitedata[i] = 0;
            }
        } else{
            uint8_t coldwhitedata[0] = {};
        }
        if(_aActive){
            uint8_t amberdata[ledCount] = {};
            for (size_t i = 0; i < ledCount; i++){
                amberdata[i] = 0;
            }
        } else{
            uint8_t amberdata[0] = {};
        }

        for (size_t l = 0; l < effectData["la"].length(); l++){
           //build Layver Start
            long procTime = millis() - startTime[l];
            long checkTime = 0;

            for (size_t i = 0; i < effectData["la"][l].length(); i++){
                if(checkTime < procTime){
                    if(String((const char*)effectData["la"][l][i]["ty"]) == "st"){
                        uint8_t reddatap[ledCount] = {};
                        uint8_t greendatap[ledCount] = {};
                        uint8_t bluedatap[ledCount] = {};
                        for (size_t e = 0; e < ledCount; e++){
                            reddatap[i] = NULL;
                            greendatap[i] = NULL;
                            bluedatap[i] = NULL;
                        }
                        buildPattern(effectData["la"][l][i]["da"], reddatap, greendatap, bluedatap);
                        for (size_t e = 0; e < ledCount; e++){
                            if(reddatap[e] >= 0){
                                reddata[e] = reddatap[e];
                                greendata[e] = greendatap[e];
                                bluedata[e] = bluedatap[e];
                            }
                        }
                    }

                    if(String((const char*)effectData["la"][l][i]["ty"]) == "fa"){
                        int oldval = i - 1;
                        if(oldval < 0){
                            oldval = effectData["la"][l].length() - 1;
                        }
                        uint8_t reddatao[ledCount] = {};
                        uint8_t greendatao[ledCount] = {};
                        uint8_t bluedatao[ledCount] = {};
                        uint8_t reddatan[ledCount] = {};
                        uint8_t greendatan[ledCount] = {};
                        uint8_t bluedatan[ledCount] = {};
                        for (size_t e = 0; e < ledCount; e++){
                            reddatao[i] = NULL;
                            greendatao[i] = NULL;
                            bluedatao[i] = NULL;
                            reddatan[i] = NULL;
                            greendatan[i] = NULL;
                            bluedatan[i] = NULL;
                        }
                        buildPattern(effectData["la"][l][i]["da"], reddatan, greendatan, bluedatan);
                        buildPattern(effectData["la"][l][oldval]["da"], reddatao, greendatao, bluedatao);
                        for (size_t e = 0; e < ledCount; e++){
                            if(reddatan[e] >= 0 || reddatao[e] >= 0){
                                if(!(reddatan[e] >= 0)){
                                    reddatan[e] = 0;
                                    greendatan[e] = 0;
                                    bluedatan[e] = 0;
                                }
                                if(!(reddatao[e] >= 0)){
                                    reddatao[e] = 0;
                                    greendatao[e] = 0;
                                    bluedatao[e] = 0;
                                }

                                int unit = map(procTime, checkTime, checkTime + (long)effectData["la"][l][i]["du"], 0, 100000);

                                if(unit < 0){
                                    unit = 0;
                                }
                                if(unit > 100000){
                                    unit = 100000;
                                }

                                reddata[e] = map(unit, 0, 100000, reddatao[e], reddatan[e]);
                                greendata[e] = map(unit, 0, 100000, greendatao[e], greendatan[e]);
                                bluedata[e] = map(unit, 0, 100000, bluedatao[e], bluedatan[e]);
                            }
                        }
                        
                    }
                    if(String((const char*)effectData["la"][l][i]["ty"]) == "sl"){
                        int fp = (int)effectData["la"][l][i]["fp"];
                        int tp = (int)effectData["la"][l][i]["tp"];
                        fp = map(fp, 0, 1000, 0, ledCount - 1);
                        tp = map(tp, 0, 1000, 0, ledCount - 1);
                        uint8_t reddatan[ledCount] = {};
                        uint8_t greendatan[ledCount] = {};
                        uint8_t bluedatan[ledCount] = {};
                        for (size_t e = 0; e < ledCount; e++){
                            reddatan[i] = NULL;
                            greendatan[i] = NULL;
                            bluedatan[i] = NULL;
                        }
                        buildPattern(effectData["la"][l][i]["da"], reddatan, greendatan, bluedatan);
                        for (size_t e = 0; e < ledCount; e++){
                            if(reddatan[e] >= 0){
                                int unit = map(procTime, checkTime, checkTime + (long)effectData["la"][l][i]["du"], 0, 100000);

                                if(unit < 0){
                                    unit = 0;
                                }
                                if(unit > 100000){
                                    unit = 100000;
                                }
                                int pos = e + map(unit, 0, 100000, fp, tp);

                                if(pos < 0){
                                    pos = ledCount + pos;
                                }

                                if(pos > ledCount - 1){
                                    pos = pos - ledCount;
                                }

                                reddata[pos] = reddatan[e];
                                greendata[pos] = greendatan[e];
                                bluedata[pos] = bluedatan[e];
                            }
                        }                        
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

        /*for (size_t i = 0; i < ledCount; i++){
            if(_rActive)
            reddata[i] = (reddata[i] * effectBrightness) / 100;
            if(_gActive)
            greendata[i] = (greendata[i] * effectBrightness) / 100;
            bluedata[i] = (bluedata[i] * effectBrightness) / 100;
        }*/

        if(updateFunctionRGB) updateFunctionRGB(0,reddata,greendata,bluedata, warmwhitedata, normalwhitedata, coldwhitedata, amberwhitedata);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}
