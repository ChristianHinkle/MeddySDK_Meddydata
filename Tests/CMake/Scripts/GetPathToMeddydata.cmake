# Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

cmake_minimum_required(VERSION 4.0)

message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("MY_TEST_EXECUTABLE_PATH: ${MY_TEST_EXECUTABLE_PATH}")
message("MY_TEST_WORKING_DIRECTORY: ${MY_TEST_WORKING_DIRECTORY}")

include(ConditionallyWarnAboutDoubleQuotesInPathVariable)

conditionally_warn_about_double_quotes_in_path_variable(MY_TEST_EXECUTABLE_PATH)
conditionally_warn_about_double_quotes_in_path_variable(MY_TEST_WORKING_DIRECTORY)

# Delete our test files from previous test runs.
file(REMOVE_RECURSE "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject")

# Create a meddyproject for us to test on. @Christian: TODO: [todo][techdebt] This code is trying to match what the C++ code does to create a valid project. Not guaranteed to remain valid compared to what the code is doing.
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir")
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/.meddyproject")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/.meddyproject/Manifest.json")

# Create a bunch of dummy files as an example of important user data that should be untouched.
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/HereIsAnExampleTextFileNextToTheProject.txt")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyExampleTextFile.txt")
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyOtherCoolTextFiles")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyOtherCoolTextFiles/Yo.txt")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyOtherCoolTextFiles/Hey.txt")

# Invoke the c++ test program. Testing on a file.
execute_process(
  COMMAND "${MY_TEST_EXECUTABLE_PATH}"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyOtherCoolTextFiles/Hey.txt"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/.meddyproject/meddydata/MyOtherCoolTextFiles/Hey.txt"
  WORKING_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}"
  RESULT_VARIABLE ResultVariable
  COMMAND_ECHO STDOUT
  )

message("ResultVariable: ${ResultVariable}")

# Exit now if the test failed within the c++ program.
if(NOT ${ResultVariable} STREQUAL 0)
  cmake_language(EXIT ${ResultVariable})
endif()

# Invoke the c++ test program. Testing on a directory.
execute_process(
  COMMAND "${MY_TEST_EXECUTABLE_PATH}"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/MyOtherCoolTextFiles"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/MyProjectDir/.meddyproject/meddydata/MyOtherCoolTextFiles"
  WORKING_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}"
  RESULT_VARIABLE ResultVariable
  COMMAND_ECHO STDOUT
  )

message("ResultVariable: ${ResultVariable}")

# Exit now if the test failed within the c++ program.
if(NOT ${ResultVariable} STREQUAL 0)
  cmake_language(EXIT ${ResultVariable})
endif()

# Invoke the c++ test program. Testing on a file outside the meddyproject.
execute_process(
  COMMAND "${MY_TEST_EXECUTABLE_PATH}"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/GetOuterMeddyproject/HereIsAnExampleTextFileNextToTheProject.txt"
    ""
  WORKING_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}"
  RESULT_VARIABLE ResultVariable
  COMMAND_ECHO STDOUT
  )

message("ResultVariable: ${ResultVariable}")

# Exit now if the test failed within the c++ program.
if(NOT ${ResultVariable} STREQUAL 0)
  cmake_language(EXIT ${ResultVariable})
endif()

# Success.
cmake_language(EXIT 0)
