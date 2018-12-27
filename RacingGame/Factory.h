#ifndef FACTORY_TRAIT_H
#define FACTORY_TRAIT_H

template <typename T>
class Factory {
public:
	static T& create() = 0;
	static void release() = 0;
};

#endif // FACTORY_TRAIT_H