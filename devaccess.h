#ifndef DEVACCESS_H
#define DEVACCESS_H

#include <stdint.h>

namespace DevAccess
{
	bool open();
	void close();
	
	uint16_t write(const uint8_t* data, uint16_t len);
    uint16_t read(uint8_t* data, uint16_t len);
}

#endif // DEVACCESS_H
