#ifndef EffectEngine_h
	#define EffectEngine_h
	
	extern "C" {
		typedef void (*callbackFunction)(*int);
	}

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
		#else
		#include "WProgram.h"
	#endif 
	
	class EffectEngine{
		public:	

			EffectEngine();
			EffectEngine(int leds);
		
            void tick();
            void setData(String data);
			void setBrightness(int brightness);
			void setListener(callbackFunction function);
	
		private:
			String effectData;
			int effectBrightness = 100;
			int leds = 60;
            int effectLedSize = 1;
            int effectDelay = 50;
            bool effect = false;
			callbackFunction updateFunction;
		
	};
#endif //EffectEngine