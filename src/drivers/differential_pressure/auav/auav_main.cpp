/****************************************************************************
 *
 *   Copyright (c) 2017-2024 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "AUAV.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void AUAV::print_usage()
{
	PRINT_MODULE_USAGE_NAME("auav", "driver");
	PRINT_MODULE_USAGE_SUBCATEGORY("airspeed_sensor");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('D', "Differential pressure sensing", true);
	PRINT_MODULE_USAGE_PARAM_FLAG('A', "Absolute pressure sensing", true);
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x26);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int auav_main(int argc, char *argv[])
{
	using ThisDriver = AUAV;

	BusCLIArguments cli{true, false};
	cli.default_i2c_frequency = I2C_SPEED;
	cli.i2c_address = I2C_ADDRESS_DIFFERENTIAL;

	int ch;
	uint16_t device_type = 0;
	const char *name = MODULE_NAME;

	while ((ch = cli.getOpt(argc, argv, "DA")) != EOF) {
		switch (ch) {
		case 'D':
			device_type = DRV_DIFF_PRESS_DEVTYPE_AUAV;
			name = MODULE_NAME "_differential";
			cli.i2c_address = I2C_ADDRESS_DIFFERENTIAL;
			break;

		case 'A':
			device_type = DRV_BARO_DEVTYPE_AUAV;
			name = MODULE_NAME "_absolute";
			cli.i2c_address = I2C_ADDRESS_ABSOLUTE;
			break;
		}
	}

	const char *verb = cli.optArg();

	if (!verb || device_type == 0) {
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(name, cli, device_type);

	if (!strcmp(verb, "start")) {
		return ThisDriver::module_start(cli, iterator);

	} else if (!strcmp(verb, "stop")) {
		return ThisDriver::module_stop(iterator);

	} else if (!strcmp(verb, "status")) {
		return ThisDriver::module_status(iterator);
	}

	ThisDriver::print_usage();
	return -1;
}
