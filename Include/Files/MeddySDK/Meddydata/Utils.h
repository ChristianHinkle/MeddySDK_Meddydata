// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddydata_Export.h>
#include <boost/filesystem/path.hpp>
#include <CppUtils/Core/ExpectedResult.h>
#include <MeddySDK/Meddydata/Meddydata.h>
#include <MeddySDK/Meddyproject/Meddyproject.h>

#define MEDDYSDK_MEDDYDATA_ROOT_DIR_STRING_LITERAL "meddydata"

#define MEDDYSDK_MEDDYDATA_MANIFEST_FILENAME_STRING_LITERAL "metadata.json"

/**
 *
 */
namespace MeddySDK
{
    constexpr std::string_view MeddydataRootDirString =
        MEDDYSDK_MEDDYDATA_ROOT_DIR_STRING_LITERAL;

    constexpr std::string_view MeddydataManifestFilename =
        MEDDYSDK_MEDDYDATA_MANIFEST_FILENAME_STRING_LITERAL;

    enum class Error_GetMeddydata : unsigned char
    {
        SourcePathDoesNotExist,
        CouldntLocateOuterMeddyproject,
        NoMeddydata_DoesntExist,
        NoMeddydata_NotDirectory
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_GetMeddydata> GetMeddydata(boost::filesystem::path&& sourceFilesystemPath);

    enum class Error_GetPathToMeddydata : unsigned char
    {
        SourcePathDoesNotExist,
        CouldntLocateOuterMeddyproject
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<boost::filesystem::path, Error_GetPathToMeddydata> GetPathToMeddydata(
        boost::filesystem::path&& sourceFilesystemPath);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path GetPathToMeddydata(
        boost::filesystem::path&& meddyprojectRootPath, boost::filesystem::path&& sourcePathRelative);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path DotMeddyprojectDirToMeddydataRootDir(boost::filesystem::path&& dotMeddyprojectDir);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path MeddydataRootDirToDotMeddyprojectDir(boost::filesystem::path&& meddydataRootDir);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path MeddydataPathToMeddydataManifestPath(boost::filesystem::path&& meddydataPath);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path MeddydataManifestPathToMeddydataPath(boost::filesystem::path&& manifestMeddydataPath);

    enum class Result_QueryWhetherPathIsMeddydataRootDir : unsigned char
    {
        Yes,
        No_LeafNameIsNotEqualToMeddydata,
        No_NotImmediateChildOfDotMeddyproject
    };

    MEDDYSDK_MEDDYDATA_EXPORT bool IsMeddydataRootDir(const boost::filesystem::path& filesystemPath);
    MEDDYSDK_MEDDYDATA_EXPORT Result_QueryWhetherPathIsMeddydataRootDir QueryWhetherPathIsMeddydataRootDir(const boost::filesystem::path& filesystemPath);

    enum class Result_QueryWhetherPathIsValidMeddydata : unsigned char
    {
        Yes,
        No_DoesntExist,
        No_NotDirectory,
        No_NotUnderMeddyprojectMeddydataRootDir
    };

    MEDDYSDK_MEDDYDATA_EXPORT Result_QueryWhetherPathIsValidMeddydata QueryWhetherPathIsValidMeddydata(boost::filesystem::path&& meddydataPath);

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<boost::filesystem::path, Result_QueryWhetherPathIsValidMeddydata> GetSourceFilePathFromMeddydataPath(
        const boost::filesystem::path& meddydataPath);

    enum class Error_TryAddMeddydata : unsigned char
    {
        SourcePathDoesNotExist,
        CouldntLocateOuterMeddyproject,
        FilesystemFailedToCreateMeddydata,
        FilesystemFailedToCreateManifestFile,
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_TryAddMeddydata> TryAddMeddydata(boost::filesystem::path&& sourceFilesystemPath);

    enum class Error_AddMeddydata : unsigned char
    {
        FilesystemFailedToCreateMeddydata,
        FilesystemFailedToCreateManifestFile,
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_AddMeddydata> AddMeddydata(
        MeddySDK::Meddyproject&& meddyproject,
        boost::filesystem::path&& sourcePathRelative);
}
