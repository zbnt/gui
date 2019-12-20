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

#include <Utils.hpp>

#include <vector>

void cyclesToTime(quint64 cycles, QString &time)
{
	static const std::vector<std::pair<const char*, quint64>> convTable =
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
		quint64 step = cycles / e.second;

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

void bytesToHumanReadable(quint64 bytes, QString &res, bool dec)
{
	static const std::vector<const char*> convTableBin =
	{
		"B",   "KiB", "MiB",
		"GiB", "TiB", "PiB",
		"EiB", "ZiB", "YiB"
	};

	static const std::vector<const char*> convTableDec =
	{
		"B",   "KB", "MB",
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

void bitsToHumanReadable(quint64 bits, QString &res, bool dec)
{
	bytesToHumanReadable(bits, res, dec);

	res.chop(1);
	res.push_back('b');
}

void setDeviceProperty(QByteArray *array, quint8 devID, PropertyID propID, const QByteArray &value)
{
	array->append(MSG_MAGIC_IDENTIFIER, 4);
	appendAsBytes<quint16>(array, MSG_ID_SET_PROPERTY);
	appendAsBytes<quint16>(array, 3 + value.length());
	appendAsBytes<quint8>(array, devID);
	appendAsBytes<quint16>(array, propID);
	array->append(value);
}
