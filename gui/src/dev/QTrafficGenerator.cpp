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

#include <dev/QTrafficGenerator.hpp>

#include <QFile>

#include <Utils.hpp>
#include <Messages.hpp>

QTrafficGenerator::QTrafficGenerator(QObject *parent)
	: QAbstractDevice(parent)
{ }

QTrafficGenerator::~QTrafficGenerator()
{ }

void QTrafficGenerator::setExtraInfo(quint64 values)
{
	m_ports = values & 0xFF;
	m_maxTemplateLength = (values >> 16) & 0xFFFF;
}

void QTrafficGenerator::enableLogging(const QString &path)
{
	Q_UNUSED(path);
}

void QTrafficGenerator::disableLogging()
{ }

void QTrafficGenerator::updateDisplayedValues()
{ }

void QTrafficGenerator::receiveMeasurement(const QByteArray &measurement)
{
	Q_UNUSED(measurement);
}

void QTrafficGenerator::resetMeasurement()
{ }

QString QTrafficGenerator::description() const
{
	return QString("Traffic generator (eth%1)").arg(m_ports & 0xFF);
}

QString QTrafficGenerator::settingsQml() const
{
	return QString("qrc:/qml/SettingsTabTG.qml");
}

QString QTrafficGenerator::statusQml() const
{
	return QString("");
}

bool QTrafficGenerator::loadTemplate(QUrl url)
{
	QString selectedPath = url.toLocalFile();

	if(!selectedPath.length())
	{
		emit error("No file selected");
		return false;
	}

	QFile headersFile;
	headersFile.setFileName(selectedPath);
	headersFile.open(QIODevice::ReadOnly);

	if(!headersFile.isOpen())
	{
		emit error("Can't read template file, make sure you have the required permissions");
		return false;
	}

	QByteArray templateBytes, templateMask;

	if(selectedPath.endsWith(".hex"))
	{
		QByteArray fileContents = headersFile.readAll();
		quint8 num = 0x10, maskCount = 0;
		quint16 maskOffset = 0;
		bool inX = false;

		templateMask.append(0xFF);

		for(char c : fileContents)
		{
			if(inX)
			{
				inX = false;
				continue;
			}

			if(c == 'x' || c == 'X')
			{
				inX = true;
				templateBytes.append('\0');

				++maskCount;

				if(maskCount >= 8)
				{
					maskCount = 0;
					++maskOffset;
					templateMask.append(0xFF);
				}
			}
			else if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
			{
				c = tolower(c) - '0';

				if(c >= 10)
					c -= 'a' - '9' - 1;

				if(num <= 0xF)
				{
					templateBytes.append((num << 4) | c);
					num = 0x10;

					if(maskOffset < templateMask.size())
					{
						templateMask[maskOffset] = templateMask[maskOffset] & ~(1u << maskCount);
						++maskCount;

						if(maskCount >= 8)
						{
							maskCount = 0;
							++maskOffset;
							templateMask.append(0xFF);
						}
					}
				}
				else
				{
					num = c;
				}
			}
		}
	}
	else
	{
		templateBytes = headersFile.readAll();
	}

	if(templateBytes.size() > m_maxTemplateLength)
	{
		emit error(QString("Template can not be larger than %1 bytes").arg(m_maxTemplateLength));
		return false;
	}

	m_templateLength = templateBytes.length();
	m_templatePath = selectedPath;
	m_templateLoaded = true;
	emit templateChanged();

	return true;
}

void QTrafficGenerator::clearTemplate()
{
	m_templateBytes.clear();
	m_templateMask.clear();

	m_templateLength = 0;
	m_templatePath = "";
	m_templateLoaded = false;

	emit templateChanged();
}
