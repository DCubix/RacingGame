#ifndef BINARY_IO_H
#define BINARY_IO_H

#include <cstring>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "Int.h"
#include "Collections.h"
#include "Memory.h"

class BinReader {
public:
	explicit BinReader(u8 *data);
	~BinReader() = default;

	template <typename T = u8>
	T read() {
		T val{};
		size_t len = sizeof(T);
		std::memcpy(&val, m_data, len);
		m_data += len;
		return val;
	}

private:
	u8 *m_data;
};

class BinWriter {
public:
	BinWriter() = default;
	~BinWriter() = default;

	template <typename T = u8>
	void write(T data) {
		size_t len = sizeof(data);
		if (len > 1) {
			u8* bytes = new u8[len];
			std::memcpy(bytes, &data, len);
			m_data.insert(m_data.end(), std::begin(bytes), std::end(bytes));

			delete[] bytes;
		} else {
			m_data.push_back(data);
		}
	}

	template <typename... Ts>
	void writeAll(Ts&&... args) {
		(write(args), ...);
	}

	u8* data() { return m_data.data(); }
	size_t dataSize() const { return m_data.size(); }

private:
	Vec<u8> m_data;
};

#endif // BINARY_IO_H