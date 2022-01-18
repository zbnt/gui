/*
	zbnt/gui
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

#include <Messages.hpp>
#include <MessageUtils.hpp>

QTrafficGenerator::QTrafficGenerator(QObject *parent)
	: QAbstractDevice(DEV_TRAFFIC_GENERATOR, parent)
{ }

QTrafficGenerator::~QTrafficGenerator()
{ }

void QTrafficGenerator::loadInitialProperties(const QList<QPair<PropertyID, QByteArray>> &props)
{
	for(auto prop : props)
	{
		switch(prop.first)
		{
			case PROP_PORTS:
			{
				if(prop.second.size() < 1) break;

				m_port = readAsNumber<quint8>(prop.second, 0);
				break;
			}

			case PROP_MAX_TEMPLATE_SIZE:
			{
				if(prop.second.size() < 4) break;

				m_maxTemplateLength = readAsNumber<quint32>(prop.second, 0);
				break;
			}

			default: { }
		}
	}
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
	return QString("Traffic generator (eth%1)").arg(m_port);
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

	QByteArray templateBytes, sourceBytes;
	QByteArray fileContents = headersFile.readAll();

	bool inX = false, inN = false, inComment = false;
	quint8 lastChar = 0;
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

			templateBytes.append('\0');
			sourceBytes.append(0x01);

			inX = false;
			continue;
		}

		if(inN)
		{
			if(c != 'n' && c != 'b')
			{
				emit error(QString("Syntax error in line %1: Incomplete frame number sequence").arg(lineCount));
				return false;
			}

			templateBytes.append('\0');
			sourceBytes.append((c == 'n') ? 0x02 : 0x03);

			inN = false;
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
				sourceBytes.append(quint8(0x00));

				lastChar = 0;
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
		}
		else if(c == 'n')
		{
			inN = true;
		}
		else if(!isspace(c))
		{
			emit error(QString("Syntax error in line %1: Invalid character").arg(lineCount));
			return false;
		}
	}

	if(templateBytes.size() > m_maxTemplateLength)
	{
		emit error(QString("Template can not be larger than %1 bytes").arg(m_maxTemplateLength));
		return false;
	}

	m_templateBytes = templateBytes;
	m_sourceBytes = sourceBytes;

	m_templateLength = templateBytes.length();
	m_templatePath = selectedPath;
	m_templateLoaded = true;
	emit templateChanged();

	return true;
}

void QTrafficGenerator::clearTemplate()
{
	m_templateBytes.clear();
	m_sourceBytes.clear();

	m_templateLength = 0;
	m_templatePath = "";
	m_templateLoaded = false;

	emit templateChanged();
}
