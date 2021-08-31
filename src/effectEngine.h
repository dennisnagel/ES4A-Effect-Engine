#ifndef effectEngine_h
	#define effectEngine_h
	
	extern "C" {
		
	}

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "Arduino.h"
		#else
		#include "WProgram.h"
	#endif 
	
	class effectEngine{
		public:	

			effectEngine();
			effectEngine(int leds);
		
            void tick();
            void setData(String data);
	
		private:
			int leds = 60;
            int effectLedSize = 1;
            int effectDelay = 50;
            bool effect = false;
		
	};
#endif //effectEngine