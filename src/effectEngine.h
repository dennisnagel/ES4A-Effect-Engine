#ifndef EffectEngine_h
	#define EffectEngine_h
	
	extern "C" {
		typedef void (*callbackFunctionRGB)(uint8_t [], uint8_t [], uint8_t []);
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
		
            void tick();
            void setData(String data);
			void setBrightness(int brightness);
			void setListenerRGB(callbackFunctionRGB function);
	
		private:
			JSONVar effectData;
			int effectBrightness = 100;
			int ledCount = 60;
            int effectLedSize = 1;
            int effectDelay = 50;
            bool effect = false;
			uint8_t redarray[] = {};
        	uint8_t greenarray[] = {};
        	uint8_t bluearray[] = {};
			callbackFunctionRGB updateFunctionRGB;
		
	};
#endif //EffectEngine