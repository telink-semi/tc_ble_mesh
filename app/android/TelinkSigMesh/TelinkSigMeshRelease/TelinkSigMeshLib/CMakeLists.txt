# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# set(nativeSourceDir ~/Workspace/android/telink/TelinkSigMeshDebug/TelinkSigMeshLib/src/main/cpp/)
set(nativeSourceDir ${PROJECT_SOURCE_DIR}/src/main/cpp/)

set(meshSourceDir ${PROJECT_SOURCE_DIR}/src/main/jniLibs/)




ADD_DEFINITIONS(-DANDROID_APP_ENABLE=1)
ADD_DEFINITIONS(-DWIN32=1)

# include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp/header/)

add_library( # Sets the name of the library.
             #native-lib
             mesh_lib_share

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).

             # mesh source
            ${nativeSourceDir}/mesh_lib.c

             )

add_library(
libmesh_lib_static STATIC IMPORTED
)

set_target_properties(
libmesh_lib_static
PROPERTIES IMPORTED_LOCATION
${meshSourceDir}/${ANDROID_ABI}/libmesh_lib_static.a
)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       # native-lib
                       mesh_lib_share

                       # Links the target library to the log library
                       # included in the NDK.
                       libmesh_lib_static
                       ${log-lib} )