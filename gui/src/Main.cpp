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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>

#include <QDebug>

#include <zbnt.hpp>
#include <dev/QStatsCollector.hpp>
#include <dev/QTrafficGenerator.hpp>
#include <dev/QLatencyMeasurer.hpp>
#include <dev/QFrameDetector.hpp>

#include <QTableModel.hpp>
#include <QIPValidator.hpp>
#include <QUInt64Validator.hpp>
#include <QUInt64RangeValidator.hpp>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("zbnt");
	QCoreApplication::setOrganizationDomain("zbnt.oscar-rc.dev");
	QCoreApplication::setApplicationName("zbnt_gui");

	// Register QML types

	qmlRegisterSingletonType<ZBNT>("zbnt", 1, 0, "ZBNT",
		[](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject*
		{
			Q_UNUSED(engine)
			Q_UNUSED(scriptEngine);
			return new ZBNT();
		}
	);

	qmlRegisterType<QStatsCollector>("zbnt", 1, 0, "QStatsCollector");
	qmlRegisterType<QTrafficGenerator>("zbnt", 1, 0, "QTrafficGenerator");
	qmlRegisterType<QLatencyMeasurer>("zbnt", 1, 0, "QLatencyMeasurer");
	qmlRegisterType<QFrameDetector>("zbnt", 1, 0, "QFrameDetector");

	qmlRegisterType<QTableModel>("zbnt", 1, 0, "QTableModel");
	qmlRegisterType<QIPValidator>("zbnt", 1, 0, "IPValidator");
	qmlRegisterType<QUInt64Validator>("zbnt", 1, 0, "UInt64Validator");
	qmlRegisterType<QUInt64RangeValidator>("zbnt", 1, 0, "UInt64RangeValidator");

	qmlRegisterSingletonType(QUrl("qrc:/qml/DefaultLabel.qml"), "zbnt", 1, 0, "DefaultLabel");
	qmlRegisterSingletonType(QUrl("qrc:/qml/DisabledLabel.qml"), "zbnt", 1, 0, "DisabledLabel");

	// Start QML engine

	QQmlApplicationEngine engine;
	engine.load("qrc:/qml/Main.qml");

	if(engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
