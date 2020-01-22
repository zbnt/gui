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
	QByteArray fileContents = headersFile.readAll();

	bool inX = false, inComment = false;
	quint8 lastChar = 0, currentMask = 0, maskCount = 0;
	quint32 lineCount = 1;

	for(char c : fileContents)
	{
		c = tolower(c);

		if(inX)
		{
			if(c != 'x')
			{
				emit error(QString("Syntax error in line %1: Incomplete random byte sequence").arg(lineCount));
				return false;
			}

			inX = false;
			continue;
		}

		if(inComment)
		{
			if(c == '\n')
			{
				inComment = false;
				++lineCount;
			}

			continue;
		}

		if((c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		{
			if(lastChar)
			{
				c -= '0';
				lastChar -= '0';

				if(c >= 10)
				{
					c -= 'a' - '9' - 1;
				}

				if(lastChar >= 10)
				{
					lastChar -= 'a' - '9' - 1;
				}

				templateBytes.append((lastChar << 4) | c);
				lastChar = 0;
				++maskCount;
			}
			else
			{
				lastChar = c;
			}
		}
		else if(lastChar)
		{
			emit error(QString("Syntax error in line %1: Incomplete byte sequence").arg(lineCount));
			return false;
		}
		else if(c == '\n')
		{
			++lineCount;
		}
		else if(c == '#')
		{
			inComment = true;
		}
		else if(c == 'x')
		{
			inX = true;

			templateBytes.append('\0');
			currentMask |= 1 << maskCount;
			++maskCount;
		}
		else if(!isspace(c))
		{
			emit error(QString("Syntax error in line %1: Invalid character").arg(lineCount));
			return false;
		}

		if(maskCount >= 8)
		{
			templateMask.append(currentMask);
			currentMask = 0;
			maskCount = 0;
		}
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
