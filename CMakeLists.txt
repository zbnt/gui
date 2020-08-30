cmake_minimum_required(VERSION 3.7)
project(zbnt_gui)

find_package(Qt5 COMPONENTS Core Gui Network Qml Widgets REQUIRED)

set(ZBNT_FALLBACK_VERSION "2.0.0-beta.2")
set(CMAKE_CXX_STANDARD 14)

option(USE_SANITIZERS "Compile with ASan and UBSan" OFF)

if(USE_SANITIZERS)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -fsanitize=address -fsanitize=undefined")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fpermissive -fsanitize=address -fsanitize=undefined")
else()
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fpermissive")
endif()

add_custom_target(
	zbnt_genver ALL
	COMMAND "python3" "cmake/GenerateVersion.py" "${CMAKE_BINARY_DIR}" "${ZBNT_FALLBACK_VERSION}"
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	BYPRODUCTS "${CMAKE_BINARY_DIR}/Version.txt" "${CMAKE_BINARY_DIR}/Version.cpp"
	COMMENT "Generating version files"
)

set(ZBNT_GUI_SRC
	"src/Main.cpp"
	"src/QTableModel.cpp"

	"src/zbnt.cpp"
	"src/Utils.cpp"
	"src/QNetworkWorker.cpp"
	"src/QDiscoveryClient.cpp"

	"src/QIPValidator.cpp"
	"src/QUInt64Validator.cpp"
	"src/QUInt64RangeValidator.cpp"

	"src/dev/QAbstractDevice.cpp"
	"src/dev/QStatsCollector.cpp"
	"src/dev/QTrafficGenerator.cpp"
	"src/dev/QLatencyMeasurer.cpp"
	"src/dev/QFrameDetector.cpp"

	"server-shared/src/MessageUtils.cpp"
	"server-shared/src/MessageReceiver.cpp"
	"${CMAKE_BINARY_DIR}/Version.cpp"
)

set(ZBNT_GUI_HDR
	"include/QTableModel.hpp"

	"include/zbnt.hpp"
	"include/QNetworkWorker.hpp"
	"include/QDiscoveryClient.hpp"

	"include/QIPValidator.hpp"
	"include/QUInt64Validator.hpp"
	"include/QUInt64RangeValidator.hpp"

	"include/dev/QAbstractDevice.hpp"
	"include/dev/QStatsCollector.hpp"
	"include/dev/QTrafficGenerator.hpp"
	"include/dev/QLatencyMeasurer.hpp"
	"include/dev/QFrameDetector.hpp"

	"server-shared/include/Messages.hpp"
)

set(ZBNT_GUI_RES
	"assets.qrc"
	"qml.qrc"
)

qt5_wrap_cpp(ZBNT_GUI_SRC_MOC ${ZBNT_GUI_HDR})
qt5_add_resources(ZBNT_GUI_SRC_RES ${ZBNT_GUI_RES})

set_source_files_properties("${CMAKE_BINARY_DIR}/Version.cpp" PROPERTIES GENERATED 1)

add_executable(zbnt_gui ${ZBNT_GUI_SRC} ${ZBNT_GUI_SRC_RES} ${ZBNT_GUI_SRC_MOC})
add_dependencies(zbnt_gui zbnt_genver)
target_link_libraries(zbnt_gui Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml Qt5::Widgets -lpthread)
target_include_directories(zbnt_gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/server-shared/include" "${CMAKE_BINARY_DIR}")

install(
	TARGETS zbnt_gui
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
	DESTINATION bin
)
