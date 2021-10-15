#ifndef EffectEngine_h
	#define EffectEngine_h
	
	extern "C" {
		
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
	
		private:
			int leds = 60;
            int effectLedSize = 1;
            int effectDelay = 50;
            bool effect = false;
		
	};
#endif //EffectEngine