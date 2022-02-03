#ifndef EffectEngine_h
	#define EffectEngine_h

	extern "C" {
		typedef void (*callbackFunctionRGB)(int pos, uint8_t r[], uint8_t g[], uint8_t b[], uint8_t ww[], uint8_t nw[], uint8_t cw[], uint8_t a[]); //rgbwwa
	}

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
		#else
		#include "WProgram.h"
	#endif 
	#include <Arduino_JSON.h>
	
	class EffectEngine{
		public:	

			EffectEngine();
			EffectEngine(int leds);
			EffectEngine(int leds, int width);
		
            void tick();
            void setData(String data);
			void setBrightness(int brightness);
			void setListenerRGB(callbackFunctionRGB function);
			void setColorMode(bool r, bool g, bool b, bool ww, bool nw, bool cw, bool a);
	
		private:
			void HsvToRgb(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b);
			void ColorToRgbwwwa(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &ww, uint8_t &nw, uint8_t &cw, uint8_t &a);
			void buildPattern(JSONVar data, uint8_t reddata[], uint8_t greendata[], uint8_t bluedata[]);

			JSONVar effectData;
			callbackFunctionRGB updateFunctionRGB;
			bool _rActvie = true;
			bool _gActvie = true;
			bool _bActvie = true;
			bool _wwActvie = false;
			bool _nwActvie = false;
			bool _cwActvie = false;
			bool _aActvie = false;
			int effectBrightness = 100;
			int ledCount = 60;
			int _width = 1;
            bool effect = false;
			long startTime[] = {};
		
	};
#endif //EffectEngine