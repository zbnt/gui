/*
	zbnt/sw
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

#include <Utils.hpp>

#include <array>
#include <initializer_list>

void cyclesToTime(uint64_t cycles, QString &time)
{
	static const std::initializer_list<std::pair<const char*, uint64_t>> convTable =
	{
		{"d", 24 * 60 * 60 * 1'000'000'000ull},
		{"h", 60 * 60 * 1'000'000'000ull},
		{"m", 60 * 1'000'000'000ull},
		{"s", 1'000'000'000ull},
		{"ms", 1'000'000},
		{"us", 1'000},
		{"ns", 1}
	};

	cycles *= 8;
	bool first = true;

	for(const auto &e : convTable)
	{
		uint64_t step = cycles / e.second;

		if(step || (first && e.second == 1))
		{
			if(!first)
			{
				time += " + ";
			}

			first = false;

			time += QString::number(step);
			time += " ";
			time += e.first;
		}

		cycles %= e.second;
	}
}

void bytesToHumanReadable(uint64_t bytes, QString &res, bool dec)
{
	static const std::array<const char*, 9> convTableBin =
	{
		"B",   "KiB", "MiB",
		"GiB", "TiB", "PiB",
		"EiB", "ZiB", "YiB"
	};

	static const std::array<const char*, 9> convTableDec =
	{
		"B",  "KB", "MB",
		"GB", "TB", "PB",
		"EB", "ZB", "YB"
	};

	int idx = 0;
	float div = bytes;

	if(!dec)
	{
		if(bytes & ~0x3FF)
		{
			while(bytes & ~0xFFFFF)
			{
				idx += 1;
				bytes >>= 10;
			}

			idx++;
			div = bytes / 1024.0;
		}

		res = QString::number(div, 'f', 2);
		res += " ";
		res += convTableBin[idx];
	}
	else
	{
		if(bytes >= 1000)
		{
			while(bytes >= 1'000'000)
			{
				idx += 1;
				bytes /= 1000;
			}

			idx++;
			div = bytes / 1000.0;
		}

		res = QString::number(div, 'f', 2);
		res += " ";
		res += convTableDec[idx];
	}
}

void bitsToHumanReadable(uint64_t bits, QString &res, bool dec)
{
	bytesToHumanReadable(bits, res, dec);

	res.chop(1);
	res.push_back('b');
}
