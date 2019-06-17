cmake_minimum_required(VERSION 3.7)
project(zbnt_gui)

find_package(Qt5 COMPONENTS Core Gui Network Qml Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(ZBNT_GUI_SRC
	"src/Main.cpp"
	"src/Utils.cpp"

	"src/zbnt.cpp"
	"src/MessageReceiver.cpp"

	"src/QStatsCollector.cpp"
	"src/QTrafficGenerator.cpp"
	"src/QLatencyMeasurer.cpp"

	"src/QIPValidator.cpp"
	"src/QUInt64Validator.cpp"
	"src/QUInt64RangeValidator.cpp"
)

set(ZBNT_GUI_HDR
	"include/zbnt.hpp"

	"include/QStatsCollector.hpp"
	"include/QTrafficGenerator.hpp"
	"include/QLatencyMeasurer.hpp"

	"include/QIPValidator.hpp"
	"include/QUInt64Validator.hpp"
	"include/QUInt64RangeValidator.hpp"
)

set(ZBNT_GUI_RES
	"qml.qrc"
)

qt5_wrap_cpp(ZBNT_GUI_SRC_MOC ${ZBNT_GUI_HDR})
qt5_add_resources(ZBNT_GUI_SRC_RES ${ZBNT_GUI_RES})

add_executable(zbnt_gui ${ZBNT_GUI_SRC} ${ZBNT_GUI_SRC_RES} ${ZBNT_GUI_SRC_MOC})
target_link_libraries(zbnt_gui Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml Qt5::Widgets)
