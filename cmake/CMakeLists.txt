cmake_minimum_required(VERSION 2.8)

project(UTK)
#set(CMAKE_BUILD_TYPE Release ... FORCE)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -D_DEBUG")
add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1 -D_CRT_NON_CONFORMING_SWPRINTFS=1 -D_SCL_SECURE_NO_WARNINGS=1 -DNDEBUG -D_UNICODE -DNOPCH -D_XKEYCHECK_H)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../UTK/src ${CMAKE_CURRENT_SOURCE_DIR}/../UTK/include /usr/local/include/nlohmann)

set(MY_SRC "../UTK/src/main.cpp" 		"../UTK/include/main.h"
        "../UTK/src/AboutDialog.cpp"		"../UTK/include/AboutDialog.h"
        "../UTK/src/addNewRow.cpp"		"../UTK/include/addNewRow.h"
        "../UTK/src/gridFrameSource.cpp"        "../UTK/include/gridFrameSource.h"
        "../UTK/src/SettingsDialog.cpp"		"../UTK/include/SettingsDialog.h"
        "../UTK/src/sourceEditPanel.cpp"        "../UTK/include/sourceEditPanel.h"
        "../UTK/src/utkGuiFrame.cpp"		"../UTK/include/utkGuiFrame.h"
        "../UTK/src/extradition.cpp"		"../UTK/include/extradition.h"
        "../UTK/src/gridFrame.cpp"		"../UTK/include/gridFrame.h"
        "../UTK/src/logFile.cpp"		"../UTK/include/logFile.h"
        "../UTK/src/reception.cpp"		"../UTK/include/reception.h"
        "../UTK/src/sqlite.cpp"			"../UTK/include/sqlite.h"
        "../UTK/src/GridFrameBase.cpp"		"../UTK/include/GridFrameBase.h"
        "../UTK/src/BatteryPanel.cpp" 		"../UTK/include/BatteryPanel.h"
        "../UTK/src/gridFrameBattery.cpp"	"../UTK/include/gridFrameBattery.h"
        "../UTK/src/gridFrameBinds.cpp"		"../UTK/include/gridFrameBinds.h"
        "../UTK/src/AddRowDialogBind.cpp"	"../UTK/include/AddRowDialogBind.h"
        "../UTK/history.txt"			"../UTK/include/Definitions.h"
        )
    message("${MY_SRC}")

add_executable(${PROJECT_NAME} ${MY_SRC})

message("++++++++++++++++++++++++++++++++++++++++++++++++++++++")
find_package(wxWidgets REQUIRED)
if(wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${wxWidgets_DEPENDENCIES})
	message("${wxWidgets_LIBRARIES}")
endif()

set(CMAKE_MODULE_PATH "/media/data/Project/nix/QT/cmakeModule")
#find_package(wxsqlite3 REQUIRED wxcode_gtk3u_wxsqlite3-3.1)
if(wxsqlite3_FOUND)
#set(wxsqlite3_DEFINITIONS ${wxsqlite3_CFLAGS_OTHER})
	message("----------------------------------------------")
#find_path(wxsqlite3_INCLUDE_DIR wxsqlite3.h HINTS ${wxsqlite3_INCLUDEDIR} ${wxsqlite3_INCLUDE_DIR})
	include_directories(${wxsqlite3_INCLUDE_DIRS})
	link_directories(${wxsqlite3_LIBRARY_DIRS})
	target_link_libraries(${PROJECT_NAME} ${wxsqlite3_LIBRARIES})
	message("${wxsqlite3_FOUND}")
	message("${PROJECT_NAME}")
	message("${wxsqlite3_INCLUDE_DIR}")
	message("${wxsqlite3_LIBRARY_DIRS}")
	message("${wxsqlite3_LIBRARIES}")
	message("${wxsqlite3_DEPENDENCIES}")
endif()
include_directories("/usr/local/include")
link_directories("/usr/local/lib")
target_link_libraries(${PROJECT_NAME} "xlsxwriter" "wxcode_gtk3u_wxsqlite3-3.1" "z")
