# Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

cmake_minimum_required(VERSION 4.0)

# Write out the main project name here, in one place, to make renaming easier. This is mainly used by our
# boilerplate code. There may be other mentions of the project's name mixed in certain places of this file, so
# don't rely on these variables alone for renaming.
SET(MY_BASE_PROJECT_NAME_FULL "MeddySDK_Meddydata")
string(TOLOWER ${MY_BASE_PROJECT_NAME_FULL} MY_BASE_PROJECT_NAME_FULL_LOWERCASE)
string(TOUPPER ${MY_BASE_PROJECT_NAME_FULL} MY_BASE_PROJECT_NAME_FULL_UPPERCASE)
SET(MY_BASE_PROJECT_NAME_NAMESPACE "MeddySDK")
SET(MY_BASE_PROJECT_NAME_LEAFNAME "Meddydata")

project(${MY_BASE_PROJECT_NAME_FULL}_Tests
  VERSION 0.1.0
  LANGUAGES CXX
  )

find_package(MeddySDK_Meddyproject CONFIG REQUIRED COMPONENTS Static)
find_package(Boost 0 CONFIG REQUIRED COMPONENTS filesystem)

# Note that we do not "find package" for our parent project. We don't need to since we are built in the same
# cmake invocation as the it. That means we're being processed during the same configuration step as them, which
# means we'll have all their targets. Also, the targets that we reference in `target_link_libraries` commands don't
# even need to exist yet because CMake only validates the existence of targets at the end of the entire
# configuration process.

add_executable(${MY_BASE_PROJECT_NAME_FULL}_GetPathToMeddydata)
target_compile_features(${MY_BASE_PROJECT_NAME_FULL}_GetPathToMeddydata PUBLIC cxx_std_20)
target_sources(${MY_BASE_PROJECT_NAME_FULL}_GetPathToMeddydata PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/GetPathToMeddydata.cpp")
target_link_libraries(${MY_BASE_PROJECT_NAME_FULL}_GetPathToMeddydata
  PRIVATE
    ${MY_BASE_PROJECT_NAME_NAMESPACE}::${MY_BASE_PROJECT_NAME_LEAFNAME}::Static
    Boost::filesystem
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.GetPathToMeddydata
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_GetPathToMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/GetPathToMeddydata.cmake"
  )

add_executable(${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata)
target_compile_features(${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata PUBLIC cxx_std_20)
target_sources(${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source/TryAddMeddydata.cpp")
target_link_libraries(${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata
  PRIVATE
    ${MY_BASE_PROJECT_NAME_NAMESPACE}::${MY_BASE_PROJECT_NAME_LEAFNAME}::Static
    Boost::filesystem
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.SuccessCase.OnFile
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_SuccessCase_OnFile.cmake"
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.SuccessCase.OnDirectory
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_SuccessCase_OnDirectory.cmake"
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.FailCase.NoMeddyproject
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_FailCase_NoMeddyproject.cmake"
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.FailCase.SourceFileDoesNotExist
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_FailCase_SourceFileDoesNotExist.cmake"
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.FailCase.ConflictingMeddydataFileObject
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_FailCase_ConflictingMeddydataFileObject.cmake"
  )

add_test(
  NAME ${MY_BASE_PROJECT_NAME_NAMESPACE}.${MY_BASE_PROJECT_NAME_LEAFNAME}.TryAddMeddydata.FailCase.ConflictingManifestFileObject
  COMMAND ${CMAKE_COMMAND}
    "-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}"
    "-DMY_TEST_EXECUTABLE_PATH=$<TARGET_FILE:${MY_BASE_PROJECT_NAME_FULL}_TryAddMeddydata>"
    "-DMY_TEST_WORKING_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Scripts/TryAddMeddydata_FailCase_ConflictingManifestFileObject.cmake"
  )
