
find_package(Qt5 COMPONENTS Core Gui Network Qml Widgets REQUIRED)

set(ZBNT_GUI_SRC
	"src/Main.cpp"
	"src/QTableModel.cpp"

	"src/zbnt.cpp"
	"src/QNetworkWorker.cpp"
	"src/MessageReceiver.cpp"
	"src/QDiscoveryClient.cpp"

	"src/QStatsCollector.cpp"
	"src/QTrafficGenerator.cpp"
	"src/QLatencyMeasurer.cpp"
	"src/QFrameDetector.cpp"

	"src/QIPValidator.cpp"
	"src/QUInt64Validator.cpp"
	"src/QUInt64RangeValidator.cpp"

	"../common/src/Utils.cpp"
)

set(ZBNT_GUI_HDR
	"include/QTableModel.hpp"

	"include/zbnt.hpp"
	"include/QNetworkWorker.hpp"
	"include/QDiscoveryClient.hpp"

	"include/QStatsCollector.hpp"
	"include/QTrafficGenerator.hpp"
	"include/QLatencyMeasurer.hpp"
	"include/QFrameDetector.hpp"

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
target_link_libraries(zbnt_gui Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml Qt5::Widgets -lpthread)
target_include_directories(zbnt_gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../common/include")

target_compile_definitions(
	zbnt_gui PUBLIC
	ZBNT_VERSION="${PROJECT_VERSION}"
	ZBNT_VERSION_INT=${ZBNT_VERSION_INT}
)

install(
	TARGETS zbnt_gui
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
	DESTINATION bin
)
