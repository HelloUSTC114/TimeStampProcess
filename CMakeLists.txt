# ----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.21)
project(ProcessTimeStamp)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(LINKDEFHEADER ${PROJECT_SOURCE_DIR}/include/LinkDef.h)
set(LIBNAME ReadFile)
set(LINKDEFDIR lkdef)
set(ROOTCLINGFILE ${LIBNAME}Dict.cxx)

# ----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
# option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

# if(WITH_GEANT4_UIVIS)
# find_package(Geant4 REQUIRED ui_all vis_all)
# else()
# find_package(Geant4 REQUIRED)
# endif()
find_package(ROOT REQUIRED COMPONENTS Core)

# find_package (Python REQUIRED COMPONENTS Interpreter Development)
# find_package(PythonLibs  REQUIRED)

# ----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${ROOT_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include
)

# if(CMAKE_SYSTEM_NAME MATCHES "Windows")
# include_directories("C:\\Users\\liang\\AppData\\Local\\Programs\\Python\\Python311\\include")
# find_library(PYTHON3LIB python3.lib "C:\\Users\\liang\\AppData\\Local\\Programs\\Python\\Python311\\libs")
# find_library(PYTHON311LIB python311.lib "C:\\Users\\liang\\AppData\\Local\\Programs\\Python\\Python311\\libs")
# link_libraries(${PYTHON3LIB} ${PYTHON311LIB})
# else()
# # message("John Test: Has the system found Python lib?")
# include_directories("/usr/include/python3.8/")
# find_package(Python REQUIRED COMPONENTS Interpreter)
# link_libraries(python3.8)
# # link_libraries("/usr/local/lib/python3.8/dist-packages")
# endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

# ----------------------------------------------------------------------------
# Locate sources and headers for this project
#
# include_directories ( ${PYTHON_INCLUDE_DIRS} )
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    execute_process(COMMAND powershell root-config --cflags OUTPUT_VARIABLE CXXFLAGS)
else()
    execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE CXXFLAGS)
    string(CONCAT CXXFLAGS ${CXXFLAGS} "-g -Wall")
    string(REPLACE "\n" " " CXXFLAGS ${CXXFLAGS})

    # set(CXXFLAGS ${CXXFLAGS}\ "-g -Wall")
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

message("CXX Flags: " ${CXXFLAGS})

# add_compile_options()
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)
list(REMOVE_ITEM headers ${LINKDEFHEADER})

# ----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
link_libraries(
    ${ROOT_LIBRARIES}

    # ROOT::Core ROOT::Imt ROOT::RIO ROOT::Net ROOT::Hist ROOT::Graf ROOT::Graf3d ROOT::Gpad ROOT::ROOTDataFrame ROOT::Tree ROOT::TreePlayer ROOT::Rint ROOT::Postscript ROOT::Matrix ROOT::Physics ROOT::MathCore ROOT::Thread ROOT::ROOTVecOps
    # ROOT::Spectrum
)

# target_link_libraries(MLC ${Geant4_LIBRARIES})

# ---------------------------------------------------------------------------
# Add rootcling step for this program
# set(LIBDIR lib)
# file(MAKE_DIRECTORY ${LIBDIR})
# file(MAKE_DIRECTORY ${LINKDEFDIR})
add_custom_command(
    OUTPUT ${ROOTCLINGFILE}
    COMMAND rootcling -f ${ROOTCLINGFILE} -c ${headers} ${LINKDEFHEADER}
)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_library(${LIBNAME} STATIC ${headers} ${sources} ${ROOTCLINGFILE})
else()
    add_library(${LIBNAME} SHARED ${headers} ${sources} ${ROOTCLINGFILE})
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

add_executable(ProcessTimeStamp test.cpp)
target_link_libraries(ProcessTimeStamp ${LIBNAME})

# ----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build MLC. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
# file(GLOB_RECURSE pyscripts RELATIVE ${PROJECT_SOURCE_DIR} pyscript/**)

# file()
# set(MLC_SCRIPTS
# gui.mac
# vis.mac
# run.mac
# ${pyscripts}
# )

# foreach(_script ${MLC_SCRIPTS})
# configure_file(
# ${PROJECT_SOURCE_DIR}/${_script}
# ${PROJECT_BINARY_DIR}/${_script}
# COPYONLY
# )
# endforeach()

# ----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
# install(TARGETS MLC DESTINATION bin)
