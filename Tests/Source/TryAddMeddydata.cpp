// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>
#include <iostream>
#include <string_view>
#include <charconv>
#include <CppUtils/StdReimpl/utility.h>
#include <optional>

namespace ErrorCodes
{
    constexpr int IncorrectNumberOfArguments = 1;
    constexpr int ActualVsExpectedResultMismatch = 2;
    constexpr int ResultStringToIntegerConversionFail = 3;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Bad arguments given. Required command synopsis: `<command-name> <input-path> <expected-error>`." << '\n';
        std::cout.flush();
        return ErrorCodes::IncorrectNumberOfArguments;
    }

    const std::string_view inputPathString = argv[1];
    const std::string_view testResultExpectedString = argv[2];

    std::cout << "Input string: \"" << inputPathString << "\"." << '\n';
    std::cout << "Expected error string: \"" << testResultExpectedString << "\"." << '\n';

    std::cout << '\n';

    boost::filesystem::path inputPath = boost::filesystem::path{inputPathString}.lexically_normal();

    std::cout << "Input path: " << inputPath << "." << '\n';

    std::optional<MeddySDK::Error_TryAddMeddydata> testResultExpectedError{std::nullopt};

    if (!testResultExpectedString.empty())
    {
        unsigned char testResultExpectedInt{};
        const std::from_chars_result fromCharsResult =
            std::from_chars(testResultExpectedString.data(), testResultExpectedString.data() + testResultExpectedString.length(), testResultExpectedInt);

        const bool isFromCharsSuccess = fromCharsResult.ec == std::errc{};
        if (!isFromCharsSuccess)
        {
            std::cout << "Bad arguments given. <expected-result> must be an unsigned integer." << '\n';
            std::cout.flush();
            return ErrorCodes::ResultStringToIntegerConversionFail;
        }

        testResultExpectedError = static_cast<MeddySDK::Error_TryAddMeddydata>(testResultExpectedInt);
    }

    std::cout << '\n';

    CppUtils::ExpectedResult testResultActualResult =
        MeddySDK::TryAddMeddydata(boost::filesystem::path{inputPath});

    if (testResultActualResult.IsError())
    {
        std::cout << "Actual result is an error." << '\n';

        if (testResultExpectedError.has_value())
        {
            if (testResultActualResult.GetError() != testResultExpectedError.value())
            {
                return ErrorCodes::ActualVsExpectedResultMismatch;
            }

            return 0;
        }

        // Returned an error, which wasn't expected by the test.
        return ErrorCodes::ActualVsExpectedResultMismatch;
    }

    std::cout << "Actual result succeeded." << '\n';

    std::cout.flush();
    return 0;
}
