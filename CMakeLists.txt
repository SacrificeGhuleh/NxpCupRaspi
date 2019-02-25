cmake_minimum_required(VERSION 3.9)
project(NxpCupRaspi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
find_package(Doxygen)

add_definitions(-DNXP_PRINT_TRACE=1)
add_definitions(-DNXP_PRINT_INFO=1)
add_definitions(-DNXP_PRINT_WARN=1)
add_definitions(-DNXP_PRINT_ERROR=1)
add_definitions(-DNXP_RASPI_BUILD=1)
add_definitions(-DNXP_RASPI_UDP=1)

if (DOXYGEN_FOUND)
    set(DOXYGEN_INPUT ${SOURCE_FILES})
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
    )

    add_custom_target(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    add_custom_target(apidoc_forced
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
endif (DOXYGEN_FOUND)

set(TFC
        libraries/tfc/tfc.cpp
        libraries/tfc/tfc.h)

set(PCH
        src/pch.h
        src/pch.cpp)

set(NXP_CODE
        src/main.cpp
        src/ImageType.h
        src/LineTracer.cpp
        src/LineTracer.h
        src/Logger.h
        src/motorsState.h
        src/NxpCarAbstract.cpp
        src/NxpCarAbstract.h
        src/NxpCommunication.cpp
        src/NxpCommunication.h
        src/NxpCupXmlConfig.cpp
        src/NxpCupXmlConfig.h
        src/NxpDefines.h
        src/NxpImage.cpp
        src/NxpImage.h
        src/NxpSerialPort.cpp
        src/NxpSerialPort.h
        src/pch.cpp
        src/pch.h
        src/PID.cpp
        src/PID.h
        src/Region.h
        src/SerialPort.cpp
        src/SerialPort.h
        src/Settings.cpp
        src/Settings.h
        src/Singleton.cpp
        src/Singleton.h
        src/UDPServer.cpp
        src/UDPServer.h
        src/XmlReader.cpp
        src/XmlReader.h
        src/PID_New.cpp
        src/PID_New.h
        src/LinuxUDPServer.h
        src/LinuxUDPServer.cpp
        src/FreescaleUdpServer.cpp src/FreescaleUdpServer.h src/NxpCarRaspi.cpp src/NxpCarRaspi.h src/NxpCarFreescale.cpp src/NxpCarFreescale.h)


set(INCLUDES
        libraries/tfc)

IF (WIN32)
    include_directories(libraries/wiringPi/wiringPi)
ELSE ()
    find_library(wiringPi_LIB wiringPi)
ENDIF ()

include_directories(${INCLUDES})
link_directories(${INCLUDES})
add_executable(NxpCupRaspi ${NXP_CODE} ${PCH} ${TFC} src/colors.h src/Timer.h)
#add_executable(NxpCupRaspi src/*.h src/*.cpp ${PCH} ${TFC} src/colors.h src/Timer.h)


IF (WIN32)
ELSE ()
    target_link_libraries(NxpCupRaspi ${wiringPi_LIB})
ENDIF ()
