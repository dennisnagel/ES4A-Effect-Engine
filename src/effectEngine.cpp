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

void EffectEngine::setColorMode(bool r, bool g, bool b, bool ww, bool nw, bool cw, bool a){
    _rActvie = r;
	_gActvie = g;
	_bActvie = b;
	_wwActvie = ww;
	_nwActvie = nw;
	_cwActvie = cw;
	_aActvie = a;
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
    else if(data.charAt(0) == 'i'){
        data.remove(0, 1);
        JSONVar newData = JSON.parse(data);
        lp = true;
        effect = false;
        startTime[0] = millis();
    }
    else{
        JSONVar newData = JSON.parse(data);
        effect = false;
        lp = false;
        if(JSON.stringify(newData) != JSON.stringify(effectData)){
            effectData = newData;
            uint8_t color[7] = {0, 0, 0, 0, 0, 0, 0};
            ColorToRgbwwwa(effectData, color[0], color[1], color[2], color[3], color[4], color[5], color[6]);
            uint8_t redarray[ledCount] = {};
            uint8_t greenarray[ledCount] = {};
            uint8_t bluearray[ledCount] = {};
            uint8_t warmwhitearray[ledCount] = {};
            uint8_t normalwhitearray[ledCount] = {};
            uint8_t coldwhitearray[ledCount] = {};
            uint8_t amberarray[ledCount] = {};

            color[0] = (color[0] * effectBrightness) / 100;
            color[1] = (color[1] * effectBrightness) / 100;
            color[2] = (color[2] * effectBrightness) / 100;
            color[3] = (color[3] * effectBrightness) / 100;
            color[4] = (color[4] * effectBrightness) / 100;
            color[5] = (color[5] * effectBrightness) / 100;
            color[6] = (color[6] * effectBrightness) / 100;

            for (size_t i = 0; i < ledCount; i++){
                redarray[i] = color[0];
                greenarray[i] = color[1];
                bluearray[i] = color[2];
                warmwhitearray[i] = color[3];
                normalwhitearray[i] = color[4];
                coldwhitearray[i] = color[5];
                amberarray[i] = color[6];
            }
            int currentWidth = 0;
            bool swt = true;
            for(int i = 0; i < _width; i++){
                if(updateFunctionRGB) updateFunctionRGB(currentWidth, redarray, greenarray, bluearray, warmwhitearray, normalwhitearray, coldwhitearray, amberarray);
                if(swt){
                    currentWidth = abs(currentWidth) + 1;
                    swt = false;
                } else{
                    currentWidth = currentWidth * -1;
                    swt = true;
                }
            }

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
        uint8_t color[7] = {0, 0, 0, 0, 0, 0, 0};
        ColorToRgbwwwa(effectData, color[0], color[1], color[2], color[3], color[4], color[5], color[6]);
        uint8_t redarray[ledCount] = {};
        uint8_t greenarray[ledCount] = {};
        uint8_t bluearray[ledCount] = {};
        uint8_t warmwhitearray[ledCount] = {};
        uint8_t normalwhitearray[ledCount] = {};
        uint8_t coldwhitearray[ledCount] = {};
        uint8_t amberarray[ledCount] = {};
        
        color[0] = (color[0] * effectBrightness) / 100;
        color[1] = (color[1] * effectBrightness) / 100;
        color[2] = (color[2] * effectBrightness) / 100;
        color[3] = (color[3] * effectBrightness) / 100;
        color[4] = (color[4] * effectBrightness) / 100;
        color[5] = (color[5] * effectBrightness) / 100;
        color[6] = (color[6] * effectBrightness) / 100;
        
        for (size_t i = 0; i < ledCount; i++){
            redarray[i] = color[0];
            greenarray[i] = color[1];
            bluearray[i] = color[2];
            warmwhitearray[i] = color[3];
            normalwhitearray[i] = color[4];
            coldwhitearray[i] = color[5];
            amberarray[i] = color[6];
        }
        int currentWidth = 0;
        bool swt = true;
        for(int i = 0; i < _width; i++){
            if(updateFunctionRGB) updateFunctionRGB(currentWidth, redarray, greenarray, bluearray, warmwhitearray, normalwhitearray, coldwhitearray, amberarray);
            if(swt){
                currentWidth = abs(currentWidth) + 1;
                swt = false;
            } else{
                currentWidth = currentWidth * -1;
                swt = true;
            }
        }
    }
}

void EffectEngine::HsvToRgb(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b){
    double hue, saturation, value;

    hue = (int)data["h"];
    saturation = (int)data["s"];
    value = (int)data["v"];

    hue = hue / 3600;
    saturation = saturation / 1000;
    value = value / 1000;

    ColorConverter::HsvToRgb(hue, saturation, value, r, g, b);
}

void EffectEngine::ColorToRgbwwwa(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &ww, uint8_t &nw, uint8_t &cw, uint8_t &a){
    if(data.hasOwnProperty("h") && data.hasOwnProperty("s") && data.hasOwnProperty("v")){
        HsvToRgb(data, r, g, b);
        ww = 0;
        nw = 0;
        cw = 0;
        a = 0;
    }
    if(data.hasOwnProperty("r") || data.hasOwnProperty("g") || data.hasOwnProperty("b") || data.hasOwnProperty("ww") || data.hasOwnProperty("nw") || data.hasOwnProperty("cw") || data.hasOwnProperty("a")){
        r = 0;
        g = 0;
        b = 0;
        ww = 0;
        nw = 0;
        cw = 0;
        a = 0;
        if(data.hasOwnProperty("r"))
            r = (int)data["r"];
        if(data.hasOwnProperty("g"))
            g = (int)data["g"];
        if(data.hasOwnProperty("b"))
            b = (int)data["b"];
        if(data.hasOwnProperty("ww"))
            ww = (int)data["ww"];
        if(data.hasOwnProperty("nw"))
            nw = (int)data["nw"];
        if(data.hasOwnProperty("cw"))
            cw = (int)data["cw"];
        if(data.hasOwnProperty("a"))
            a = (int)data["a"];
    }
}

void EffectEngine::buildPattern(JSONVar data, uint8_t reddata[], uint8_t greendata[], uint8_t bluedata[],  uint8_t warmwhitedata[], uint8_t normalwhitedata[], uint8_t coldwhitedata[], uint8_t amberdata[], int width){
    for (size_t p = 0; p < data.length(); p++){
        bool build = false;
        if(data[p].hasOwnProperty("w")){
            for(int index = 0; index < data[p]["w"].length(); index++){
                if((int)data[p]["w"][index] == width){
                    build = true;
                }
            }
            
        } else{
            build = true;
        }
        if(build){
            if(String((const char*)data[p]["ty"]) == "oc"){
                uint8_t red, green, blue, warmwhite, normalwhite, coldwhite, amber;
                ColorToRgbwwwa(data[p]["co"], red, green, blue, warmwhite, normalwhite, coldwhite, amber);
                int start = map((int)data[p]["st"], 0, 1000, 0, ledCount);
                for (size_t e = 0; e < map((int)data[p]["en"], 0, 1000, 0, ledCount) - start; e++){
                    reddata[e + start] = red; 
                    greendata[e + start] = green;
                    bluedata[e + start] = blue;
                    warmwhitedata[e + start] = warmwhite; 
                    normalwhitedata[e + start] = normalwhite;
                    coldwhitedata[e + start] = coldwhite;
                    amberdata[e + start] = amber; 
                }
            }
            if(String((const char*)data[p]["ty"]) == "fa"){
                uint8_t red1, green1, blue1, warmwhite1, normalwhite1, coldwhite1, amber1, red2, green2, blue2, warmwhite2, normalwhite2, coldwhite2, amber2;
                ColorToRgbwwwa(data[p]["cs"], red1, green1, blue1, warmwhite1, normalwhite1, coldwhite1, amber1);
                ColorToRgbwwwa(data[p]["ce"], red2, green2, blue2, warmwhite2, normalwhite2, coldwhite2, amber2);
                //HsvToRgb(data[p]["cs"], red1, green1, blue1);
                //HsvToRgb(data[p]["ce"], red2, green2, blue2);
                int end = map((int)data[p]["en"], 0, 1000, 0, ledCount);
                int start = map((int)data[p]["st"], 0, 1000, 0, ledCount);
                int count = end - start;
                for (size_t e = 0; e < count; e++){
                    reddata[e + start] = map(e, 0, count, red1, red2);
                    greendata[e + start] = map(e, 0, count, green1, green2);
                    bluedata[e + start] = map(e, 0, count, blue1, blue2);
                    warmwhitedata[e + start] = map(e, 0, count, warmwhite1, warmwhite2);
                    normalwhitedata[e + start] = map(e, 0, count, normalwhite1, normalwhite2);
                    coldwhitedata[e + start] = map(e, 0, count, coldwhite1, coldwhite2);
                    amberdata[e + start] = map(e, 0, count, amber1, amber2);
                }
            }
        }
    }
}

void EffectEngine::runWidth(int width){
    uint8_t reddata[ledCount] = {};
    uint8_t greendata[ledCount] = {};
    uint8_t bluedata[ledCount] = {};
    uint8_t warmwhitedata[ledCount] = {};
    uint8_t normalwhitedata[ledCount] = {};
    uint8_t coldwhitedata[ledCount] = {};
    uint8_t amberdata[ledCount] = {};

    for (size_t i = 0; i < ledCount; i++){
        reddata[i] = 0;
        greendata[i] = 0;
        bluedata[i] = 0;
        warmwhitedata[i] = 0;
        normalwhitedata[i] = 0;
        coldwhitedata[i] = 0;
        amberdata[i] = 0;
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
                    uint8_t warmwhitedatap[ledCount] = {};
                    uint8_t normalwhitedatap[ledCount] = {};
                    uint8_t coldwhitedatap[ledCount] = {};
                    uint8_t amberdatap[ledCount] = {};
                    for (size_t e = 0; e < ledCount; e++){
                        reddatap[i] = NULL;
                        greendatap[i] = NULL;
                        bluedatap[i] = NULL;
                        warmwhitedatap[i] = NULL;
                        normalwhitedatap[i] = NULL;
                        coldwhitedatap[i] = NULL;
                        amberdatap[i] = NULL;
                    }
                    buildPattern(effectData["la"][l][i]["da"], reddatap, greendatap, bluedatap, warmwhitedatap, normalwhitedatap, coldwhitedatap, amberdatap, width);
                    for (size_t e = 0; e < ledCount; e++){
                        if(reddatap[e] >= 0){
                            reddata[e] = reddatap[e];
                            greendata[e] = greendatap[e];
                            bluedata[e] = bluedatap[e];
                            warmwhitedata[e] = warmwhitedatap[e];
                            normalwhitedata[e] = normalwhitedatap[e];
                            coldwhitedata[e] = coldwhitedatap[e];
                            amberdata[e] = amberdatap[e];
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
                    uint8_t warmwhitedatao[ledCount] = {};
                    uint8_t normalwhitedatao[ledCount] = {};
                    uint8_t coldwhitedatao[ledCount] = {};
                    uint8_t amberdatao[ledCount] = {};

                    uint8_t reddatan[ledCount] = {};
                    uint8_t greendatan[ledCount] = {};
                    uint8_t bluedatan[ledCount] = {};
                    uint8_t warmwhitedatan[ledCount] = {};
                    uint8_t normalwhitedatan[ledCount] = {};
                    uint8_t coldwhitedatan[ledCount] = {};
                    uint8_t amberdatan[ledCount] = {};
                    for (size_t e = 0; e < ledCount; e++){
                        reddatao[e] = NULL;
                        greendatao[e] = NULL;
                        bluedatao[e] = NULL;
                        warmwhitedatao[e] = NULL;
                        normalwhitedatao[e] = NULL;
                        coldwhitedatao[e] = NULL;
                        amberdatao[e] = NULL;

                        reddatan[e] = NULL;
                        greendatan[e] = NULL;
                        bluedatan[e] = NULL;
                        warmwhitedatan[e] = NULL;
                        normalwhitedatan[e] = NULL;
                        coldwhitedatan[e] = NULL;
                        amberdatan[e] = NULL;
                    }
                    buildPattern(effectData["la"][l][i]["da"], reddatan, greendatan, bluedatan, warmwhitedatan, normalwhitedatan, coldwhitedatan, amberdatan, width);
                    buildPattern(effectData["la"][l][oldval]["da"], reddatao, greendatao, bluedatao, warmwhitedatao, normalwhitedatao, coldwhitedatao, amberdatao, width);
                    for (size_t e = 0; e < ledCount; e++){
                        if(reddatan[e] >= 0 || reddatao[e] >= 0){
                            if(!(reddatan[e] >= 0)){
                                reddatan[e] = 0;
                                greendatan[e] = 0;
                                bluedatan[e] = 0;
                                warmwhitedatan[e] = 0;
                                normalwhitedatan[e] = 0;
                                coldwhitedatan[e] = 0;
                                amberdatan[e] = 0;
                            }
                            if(!(reddatao[e] >= 0)){
                                reddatao[e] = 0;
                                greendatao[e] = 0;
                                bluedatao[e] = 0;
                                warmwhitedatao[e] = 0;
                                normalwhitedatao[e] = 0;
                                coldwhitedatao[e] = 0;
                                amberdatao[e] = 0;
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
                            warmwhitedata[e] = map(unit, 0, 100000, warmwhitedatao[e], warmwhitedatan[e]);
                            normalwhitedata[e] = map(unit, 0, 100000, normalwhitedatao[e], normalwhitedatan[e]);
                            coldwhitedata[e] = map(unit, 0, 100000, coldwhitedatao[e], coldwhitedatan[e]);
                            amberdata[e] = map(unit, 0, 100000, amberdatao[e], amberdatan[e]);
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
                    uint8_t warmwhitedatan[ledCount] = {};
                    uint8_t normalwhitedatan[ledCount] = {};
                    uint8_t coldwhitedatan[ledCount] = {};
                    uint8_t amberdatan[ledCount] = {};
                    for (size_t e = 0; e < ledCount; e++){
                        reddatan[i] = NULL;
                        greendatan[i] = NULL;
                        bluedatan[i] = NULL;
                        warmwhitedatan[i] = NULL;
                        normalwhitedatan[i] = NULL;
                        coldwhitedatan[i] = NULL;
                        amberdatan[i] = NULL;
                    }
                    buildPattern(effectData["la"][l][i]["da"], reddatan, greendatan, bluedatan, warmwhitedatan, normalwhitedatan, coldwhitedatan, amberdatan, width);
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
                            warmwhitedata[pos] = warmwhitedatan[e];
                            normalwhitedata[pos] = normalwhitedatan[e];
                            coldwhitedata[pos] = coldwhitedatan[e];
                            amberdata[pos] = amberdatan[e];
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

    for (size_t i = 0; i < ledCount; i++){
        reddata[i] = (reddata[i] * effectBrightness) / 100;
        greendata[i] = (greendata[i] * effectBrightness) / 100;
        bluedata[i] = (bluedata[i] * effectBrightness) / 100;
        warmwhitedata[i] = (warmwhitedata[i] * effectBrightness) / 100;
        normalwhitedata[i] = (normalwhitedata[i] * effectBrightness) / 100;
        coldwhitedata[i] = (coldwhitedata[i] * effectBrightness) / 100;
        amberdata[i] = (amberdata[i] * effectBrightness) / 100;
    }

    if(updateFunctionRGB) updateFunctionRGB(width, reddata, greendata, bluedata, warmwhitedata, normalwhitedata, coldwhitedata, amberdata);
}

void EffectEngine::tick(){
    if(effect){
        int currentWidth = 0;
        bool swt = true;
        for(int i = 0; i < _width; i++){
            runWidth(currentWidth);
            if(swt){
                currentWidth = abs(currentWidth) + 1;
                swt = false;
            } else{
                currentWidth = currentWidth * -1;
                swt = true;
            }
        }
    }
    if(lp){
        int steptime = effectData["data"].length() / (int) effectData["time"];
        int timeago = millis() - startTime[0];
        int step = timeago/steptime;
        uint8_t reddata[ledCount] = {};
        uint8_t greendata[ledCount] = {};
        uint8_t bluedata[ledCount] = {};
        uint8_t warmwhitedata[ledCount] = {};
        uint8_t normalwhitedata[ledCount] = {};
        uint8_t coldwhitedata[ledCount] = {};
        uint8_t amberdata[ledCount] = {};

        for (size_t i = 0; i < ledCount; i++){
            reddata[i] = 0;
            greendata[i] = 0;
            bluedata[i] = 0;
            warmwhitedata[i] = 0;
            normalwhitedata[i] = 0;
            coldwhitedata[i] = 0;
            amberdata[i] = 0;
        }

        for (size_t hightIndex = 0; hightIndex < effectData["data"][step].length(); hightIndex++){
            reddata[hightIndex] = effectData["data"][step][hightIndex][0];
            greendata[hightIndex] = effectData["data"][step][hightIndex][1];
            bluedata[hightIndex] = effectData["data"][step][hightIndex][2];
        }

        for (size_t i = 0; i < ledCount; i++){
            reddata[i] = (reddata[i] * effectBrightness) / 100;
            greendata[i] = (greendata[i] * effectBrightness) / 100;
            bluedata[i] = (bluedata[i] * effectBrightness) / 100;
            warmwhitedata[i] = (warmwhitedata[i] * effectBrightness) / 100;
            normalwhitedata[i] = (normalwhitedata[i] * effectBrightness) / 100;
            coldwhitedata[i] = (coldwhitedata[i] * effectBrightness) / 100;
            amberdata[i] = (amberdata[i] * effectBrightness) / 100;
        }
        if(updateFunctionRGB) updateFunctionRGB(0, reddata, greendata, bluedata, warmwhitedata, normalwhitedata, coldwhitedata, amberdata);
    }
}

void EffectEngine::setListenerRGB(callbackFunctionRGB function){
    updateFunctionRGB = function;
}
