// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddydata_Export.h>
#include <boost/filesystem/path.hpp>
#include <CppUtils/Core/ExpectedResult.h>

#define MEDDYSDK_MEDDYDATA_DIR_STRING_LITERAL "meddydata"

/**
 *
 */
namespace MeddySDK
{
    constexpr std::string_view MeddydataDirString =
        MEDDYSDK_MEDDYDATA_DIR_STRING_LITERAL;

    enum class Error_GetPathToMeddydata : unsigned char
    {
        SourceFilesystemPathDoesNotExist,
        CouldntLocateOuterMeddyproject
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<boost::filesystem::path, Error_GetPathToMeddydata> GetPathToMeddydata(
        boost::filesystem::path&& sourceFilesystemPath);
}
