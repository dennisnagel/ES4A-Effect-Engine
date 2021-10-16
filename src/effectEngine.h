#ifndef EffectEngine_h
	#define EffectEngine_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
		#else
		#include "WProgram.h"
	#endif 
	#include <Arduino_JSON.h>

	extern "C" {
		typedef void (*callbackFunctionRGB)(JSONVar);
	}
	
	class EffectEngine{
		public:	

			EffectEngine();
			EffectEngine(int leds);
		
            void tick();
            void setData(String data);
			void setBrightness(int brightness);
			void setListenerRGB(callbackFunctionRGB function);
	
		private:
			void HsvToRgb(JSONVar data, uint8_t &r, uint8_t &g, uint8_t &b);
			JSONVar buildPattern(JSONVar data);
			JSONVar buildLayer(JSONVar data, int index);
			JSONVar mergePattern(JSONVar data1, JSONVar data2);

			JSONVar effectData;
			int effectBrightness = 100;
			int ledCount = 60;
            int effectDelay = 50;
            bool effect = false;
			long startTime[] = {};
			callbackFunctionRGB updateFunctionRGB;
		
	};
#endif //EffectEngine