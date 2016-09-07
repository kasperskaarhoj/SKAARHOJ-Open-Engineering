/*
 * Copyright (c) 2010 by Arduino LLC. All rights reserved.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include <SPI.h>

#define SPI_CS 10
typedef uint8_t SOCKET;

#define SPI_ETHERNET_SETTINGS SPISettings(10000000, MSBFIRST, SPI_MODE0)

#include "utility/w5500.h"

#endif
