

#ifndef AWAITABLECALLBACK_HPP
#define AWAITABLECALLBACK_HPP

extern "C" {
#include "drivers/cpu.h"
}

template <typename... arguments>
class AwaitableCallback: public Callback<arguments...> {
	private:
		volatile bool called;
	public:
		void await() {
			while (!called)
				tight_powersave();
		}
		virtual void call(arguments...) {
			called=true;
		}
};

#endif
