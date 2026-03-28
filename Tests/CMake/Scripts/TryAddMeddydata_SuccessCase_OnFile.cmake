# Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

cmake_minimum_required(VERSION 4.0)

message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message("MY_TEST_EXECUTABLE_PATH: ${MY_TEST_EXECUTABLE_PATH}")
message("MY_TEST_WORKING_DIRECTORY: ${MY_TEST_WORKING_DIRECTORY}")

include(ConditionallyWarnAboutDoubleQuotesInPathVariable)

conditionally_warn_about_double_quotes_in_path_variable(MY_TEST_EXECUTABLE_PATH)
conditionally_warn_about_double_quotes_in_path_variable(MY_TEST_WORKING_DIRECTORY)

set(MY_TEST_NAME "TryAddMeddydata_SuccessCase_OnFile")

# Delete our test files from previous test runs.
file(REMOVE_RECURSE "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}")

# Create a meddyproject for us to test on. @Christian: TODO: [todo][techdebt] This code is trying to match what the C++ code does to create a valid project. Not guaranteed to remain valid compared to what the code is doing.
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir")
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/.meddyproject")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/.meddyproject/Manifest.json")

# Create a bunch of dummy files as an example of important user data that should be untouched.
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/HereIsAnExampleTextFileNextToTheProject.txt")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyExampleTextFile.txt")
file(MAKE_DIRECTORY "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles/Yo.txt")
file(TOUCH "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles/Hey.txt")

# Invoke the c++ test program. Testing on a file.
execute_process(
  COMMAND "${MY_TEST_EXECUTABLE_PATH}"
    "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles/Hey.txt"
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

# Perform the rest of the test, verifying that the files exist.

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/.meddyproject/meddydata/MyOtherCoolTextFiles/Hey.txt")
  message("Failed. Meddydata directory does not exist.")
  cmake_language(EXIT 4)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/.meddyproject/meddydata/MyOtherCoolTextFiles/Hey.txt/metadata.json")
  message("Failed. Meddydata manifest file does not exist.")
  cmake_language(EXIT 5)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/HereIsAnExampleTextFileNextToTheProject.txt")
  message("Failed. A user's file \"HereIsAnExampleTextFileNextToTheProject.txt\" has somehow been deleted during the c++ function!")
  cmake_language(EXIT 6)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyExampleTextFile.txt")
  message("Failed. A user's file \"MyProjectDir/MyExampleTextFile.txt\" has somehow been deleted during the c++ function!")
  cmake_language(EXIT 7)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles")
  message("Failed. A user's file directory \"MyProjectDir/MyOtherCoolTextFiles\" has somehow been deleted during the c++ function!")
  cmake_language(EXIT 8)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles/Yo.txt")
  message("Failed. A user's file \"MyProjectDir/MyOtherCoolTextFiles/Yo.txt\" has somehow been deleted during the c++ function!")
  cmake_language(EXIT 9)
endif()

if(NOT EXISTS "${MY_TEST_WORKING_DIRECTORY}/MyTestGeneratedFiles/${MY_TEST_NAME}/MyProjectDir/MyOtherCoolTextFiles/Hey.txt")
  message("Failed. A user's file \"MyProjectDir/MyOtherCoolTextFiles/Hey.txt\" has somehow been deleted during the c++ function!")
  cmake_language(EXIT 10)
endif()

# Success.
cmake_language(EXIT 0)
