#ifndef MEMORY_H
#define MEMORY_H

#include <memory>

template <typename T> using UPtr = std::unique_ptr<T>;

#endif // MEMORY_H