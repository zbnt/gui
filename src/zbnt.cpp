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

#include <zbnt.hpp>

#include <QDateTime>

#include <controller.h>
#include <Utils.hpp>

ZBNT::ZBNT() : QObject(nullptr)
{
	m_tg0 = new QTrafficGenerator(this);
	m_tg1 = new QTrafficGenerator(this);
	m_lm0 = new QLatencyMeasurer(this);
	m_sc0 = new QStatsCollector(this);
	m_sc1 = new QStatsCollector(this);
	m_sc2 = new QStatsCollector(this);
	m_sc3 = new QStatsCollector(this);

	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(m_socket, &QTcpSocket::stateChanged, this, &ZBNT::onNetworkStateChanged);
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &ZBNT::onNetworkError);
	connect(m_socket, &QTcpSocket::readyRead, this, &ZBNT::onReadyRead);
}

ZBNT::~ZBNT()
{ }

void ZBNT::sendSettings()
{
	// Traffic generators

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_CFG_TG0);
	m_tg0->sendSettings(m_socket);

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_CFG_TG1);
	m_tg1->sendSettings(m_socket);

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_HEADERS_TG0);
	m_tg0->sendHeaders(m_socket);

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_HEADERS_TG1);
	m_tg1->sendHeaders(m_socket);

	// Latency measurer

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_CFG_LM0);
	m_lm0->sendSettings(m_socket);

	// Send start signal

	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_START);
	sendAsBytes<quint16>(m_socket, 8);
	sendAsBytes(m_socket, m_runTime);
}

QString ZBNT::cyclesToTime(QString cycles)
{
	QString res;
	::cyclesToTime(cycles.toULongLong(), res);
	return res;
}

void ZBNT::autodetectBoardIP()
{
	// TODO
}

void ZBNT::connectToBoard()
{
	m_socket->connectToHost(m_ip, CTL_TCP_PORT);
}

void ZBNT::disconnectFromBoard()
{
	m_socket->disconnectFromHost();
}

void ZBNT::startRun()
{
	if(m_exportResults)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

		m_sc0->enableLogging(QString("measurements_") + timeStamp + "_sc0.csv");
		m_sc1->enableLogging(QString("measurements_") + timeStamp + "_sc1.csv");
		m_sc2->enableLogging(QString("measurements_") + timeStamp + "_sc2.csv");
		m_sc3->enableLogging(QString("measurements_") + timeStamp + "_sc3.csv");
		m_lm0->enableLogging(QString("measurements_") + timeStamp + "_lm0.csv");
	}

	m_sc0->resetMeasurement();
	m_sc1->resetMeasurement();
	m_sc2->resetMeasurement();
	m_sc3->resetMeasurement();
	m_lm0->resetMeasurement();

	m_currentTime = 0;
	m_currentProgress = 0;
	emit measurementChanged();

	sendSettings();
	m_running = true;
	emit runningChanged();
}

void ZBNT::stopRun()
{
	m_socket->write(CTL_MAGIC_IDENTIFIER, 4);
	sendAsBytes<uint8_t>(m_socket, SIG_STOP);
	sendAsBytes<uint16_t>(m_socket, 0);

	onRunEnd();
}

void ZBNT::onRunEnd()
{
	m_running = 0;

	m_sc0->disableLogging();
	m_sc1->disableLogging();
	m_sc2->disableLogging();
	m_sc3->disableLogging();
	m_lm0->disableLogging();

	emit measurementChanged();
	emit runningChanged();
}

QString ZBNT::runTime()
{
	return QString::number(m_runTime);
}

void ZBNT::setRunTime(QString time)
{
	m_runTime = time.toULongLong();
}

QString ZBNT::currentTime()
{
	return QString::number(m_currentTime);
}

void ZBNT::setCurrentTime(QString time)
{
	m_currentTime = time.toULongLong();
}

void ZBNT::onNetworkStateChanged(QAbstractSocket::SocketState state)
{
	switch(state)
	{
		case QAbstractSocket::UnconnectedState:
		{
			m_connected = Disconnected;
			break;
		}

		case QAbstractSocket::ConnectingState:
		{
			m_connected = Connecting;
			break;
		}

		case QAbstractSocket::ConnectedState:
		{
			m_connected = Connected;
			break;
		}

		default: { }
	}

	emit connectedChanged();
}

void ZBNT::onNetworkError(QAbstractSocket::SocketError error)
{
	switch(error)
	{
		case QAbstractSocket::NetworkError:
		{
			emit connectionError("Unreachable address");
			break;
		}

		case QAbstractSocket::SocketTimeoutError:
		{
			emit connectionError("Timeout");
			break;
		}

		case QAbstractSocket::ConnectionRefusedError:
		{
			emit connectionError("Connection refused");
			break;
		}

		default:
		{
			emit connectionError("Unknown error");
		}
	}
}

void ZBNT::onReadyRead()
{
	QByteArray readData = m_socket->readAll();

	for(uint8_t c : readData)
	{
		switch(m_rxStatus)
		{
			case SIG_RX_MAGIC:
			{
				if(c == CTL_MAGIC_IDENTIFIER[m_rxByteCount])
				{
					m_rxByteCount++;

					if(m_rxByteCount == 4)
					{
						m_rxStatus = SIG_RX_HEADER;
						m_rxByteCount = 0;
					}
				}
				else
				{
					m_rxByteCount = 0;
				}

				break;
			}

			case SIG_RX_HEADER:
			{
				switch(m_rxByteCount)
				{
					case 0:
					{
						m_rxSigID = c;
						m_rxByteCount++;
						break;
					}

					case 1:
					{
						m_rxSigSize = c;
						m_rxByteCount++;
						break;
					}

					case 2:
					{
						m_rxSigSize |= c << 8;
						m_rxByteCount = 0;

						if(!m_rxSigSize)
						{
							m_rxStatus = SIG_RX_MAGIC;

							if(m_rxSigID == SIG_MEASUREMENTS_END)
							{
								m_currentTime = m_runTime;
								m_currentProgress = 2048;
								onRunEnd();
							}
						}
						else
						{
							m_rxStatus = SIG_RX_DATA;
						}

						break;
					}
				}

				break;
			}

			case SIG_RX_DATA:
			{
				if(m_rxSigID == SIG_MEASUREMENTS && m_rxByteCount)
				{
					if(!m_rxMeasBytesLeft)
					{
						if(c <= 3)
						{
							m_rxMeasBytesLeft = 60;
						}
						else
						{
							m_rxMeasBytesLeft = 44;
						}

						m_rxBuffer.clear();
					}

					m_rxBuffer.append(c);
					m_rxMeasBytesLeft--;

					if(!m_rxMeasBytesLeft)
					{
						quint8 type = m_rxBuffer[0];

						m_rxBuffer.remove(0, 4);

						quint64 time = readAsNumber<quint64>(m_rxBuffer, 0);

						if(time > m_currentTime)
						{
							m_currentTime = time;
							m_currentProgress = (m_currentTime / double(m_runTime)) * 2048;
						}

						switch(type)
						{
							case 0:
							{
								m_sc0->receiveMeasurement(m_rxBuffer);
								break;
							}

							case 1:
							{
								m_sc1->receiveMeasurement(m_rxBuffer);
								break;
							}

							case 2:
							{
								m_sc2->receiveMeasurement(m_rxBuffer);
								break;
							}

							case 3:
							{
								m_sc3->receiveMeasurement(m_rxBuffer);
								break;
							}

							case 4:
							{
								m_lm0->receiveMeasurement(m_rxBuffer);
								break;
							}
						}

						emit measurementChanged();
					}
				}

				m_rxByteCount++;

				if(m_rxByteCount == m_rxSigSize)
				{
					m_rxStatus = SIG_RX_MAGIC;
					m_rxByteCount = 0;
					m_rxMeasBytesLeft = 0;
				}

				break;
			}
		}
	}
}
