// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>
#include <iostream>
#include <string_view>
#include <charconv>
#include <CppUtils/StdReimpl/utility.h>

namespace ErrorCodes
{
    constexpr int IncorrectNumberOfArguments = 1;
    constexpr int ActualVsExpectedResultMismatch = 2;
    constexpr int FunctionReturnedErrorWhichWasntExpected = 3;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Bad arguments given. Required command synopsis: `<command-name> <new-project-directory-path> <expected-result>`." << '\n';
        std::cout.flush();
        return ErrorCodes::IncorrectNumberOfArguments;
    }

    const std::string_view inputPathString = argv[1];
    const std::string_view testResultExpectedString = argv[2];

    std::cout << "Input string: \"" << inputPathString << "\"." << '\n';
    std::cout << "Expected result string: \"" << testResultExpectedString << "\"." << '\n';

    std::cout << '\n';

    boost::filesystem::path inputPath = boost::filesystem::path{inputPathString}.lexically_normal();
    boost::filesystem::path testResultPathExpected = boost::filesystem::path{testResultExpectedString}.lexically_normal();

    std::cout << "Input path: " << inputPath << "." << '\n';
    std::cout << "Expected result: " << testResultPathExpected << "." << '\n';

    std::cout << '\n';

    CppUtils::ExpectedResult testResultActualResult =
        MeddySDK::GetPathToMeddydata(boost::filesystem::path{inputPath});

    if (testResultActualResult.IsError())
    {
        std::cout << "Actual result is an error." << '\n';

        if (testResultExpectedString.empty())
        {
            return 0;
        }

        return ErrorCodes::FunctionReturnedErrorWhichWasntExpected;
    }

    boost::filesystem::path actualResultDotMeddyprojectPath = testResultActualResult.GetValue();

    std::cout << "Actual result path: " << actualResultDotMeddyprojectPath << "." << '\n';

    const bool didTestPass = actualResultDotMeddyprojectPath == testResultPathExpected;
    if (!didTestPass)
    {
        std::cout.flush();
        return ErrorCodes::ActualVsExpectedResultMismatch;
    }

    std::cout.flush();
    return 0;
}
