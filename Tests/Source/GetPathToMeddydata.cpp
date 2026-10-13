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

// TODO: [todo][techdebt] Review this logic and make sure that if the test expects an error but the operation succeeded, then an error code `ActualVsExpectedResultMismatch` is returned.
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Bad arguments given. Required command synopsis: `<command-name> <input-path> <expected-result>`." << '\n';
        std::cout.flush();
        return ErrorCodes::IncorrectNumberOfArguments;
    }

    const std::string_view inputPathString = argv[1];
    const std::string_view testResultExpectedString = argv[2];

    std::cout << "Input string: \"" << inputPathString << "\"." << '\n';
    std::cout << "Expected result string: \"" << testResultExpectedString << "\"." << '\n';

    std::cout << '\n';

    std::filesystem::path inputPath = std::filesystem::path{inputPathString}.lexically_normal();
    std::filesystem::path testResultPathExpected = std::filesystem::path{testResultExpectedString}.lexically_normal();

    std::cout << "Input path: " << inputPath << "." << '\n';
    std::cout << "Expected result: " << testResultPathExpected << "." << '\n';

    std::cout << '\n';

    CppUtils::ExpectedResult testResultActualResult =
        MeddySDK::GetPathToMeddydata(std::filesystem::path{inputPath});

    if (testResultActualResult.IsError())
    {
        std::cout << "Actual result is an error." << '\n';

        if (testResultExpectedString.empty())
        {
            return 0;
        }

        return ErrorCodes::FunctionReturnedErrorWhichWasntExpected;
    }

    std::filesystem::path actualResultMeddyprojectDirPath = testResultActualResult.GetValue();

    std::cout << "Actual result path: " << actualResultMeddyprojectDirPath << "." << '\n';

    const bool didTestPass = actualResultMeddyprojectDirPath == testResultPathExpected;
    if (!didTestPass)
    {
        std::cout.flush();
        return ErrorCodes::ActualVsExpectedResultMismatch;
    }

    std::cout.flush();
    return 0;
}
