
#ifndef CALLBACK_HPP
#define CALLBACK_HPP

template <typename... arguments>
class Callback {
	public:
		virtual void call(arguments...) = 0;
};

#endif
