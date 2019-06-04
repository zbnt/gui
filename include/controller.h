/*
	zbnt_gui
	Copyright (C) 2019 Oscar R.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define CTL_TCP_PORT   		5465
#define CTL_MAGIC_IDENTIFIER  "\x4D\x60\x64\x5A"

enum SignalIDs
{
	SIG_START,
	SIG_STOP,
	SIG_CFG_TG0,
	SIG_CFG_TG1,
	SIG_CFG_LM0,
	SIG_HEADERS_TG0,
	SIG_HEADERS_TG1,
	SIG_MEASUREMENTS
};

enum RxStatus
{
	SIG_RX_MAGIC,
	SIG_RX_HEADER,
	SIG_RX_DATA
};
