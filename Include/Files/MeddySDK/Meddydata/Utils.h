// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddydata_Export.h>
#include <boost/filesystem/path.hpp>
#include <CppUtils/Core/ExpectedResult.h>
#include <MeddySDK/Meddydata/Meddydata.h>
#include <MeddySDK/Meddyproject/Meddyproject.h>

#define MEDDYSDK_FILE_TREE_DIR_STRING_LITERAL "ftree"

#define MEDDYSDK_MEDDYDATA_DIR_STRING_LITERAL "_meddydata"

#define MEDDYSDK_MEDDYDATA_MANIFEST_FILENAME_STRING_LITERAL "metadata.json"

namespace rapidjson
{
    template <typename Encoding, typename Allocator, typename StackAllocator>
    class GenericDocument;
}

/**
 *
 */
namespace MeddySDK
{
    constexpr std::string_view FileTreeDirString =
        MEDDYSDK_FILE_TREE_DIR_STRING_LITERAL;

    constexpr std::string_view MeddydataDirString =
        MEDDYSDK_MEDDYDATA_DIR_STRING_LITERAL;

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

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path DotMeddyprojectDirToFileTreeDir(boost::filesystem::path&& dotMeddyprojectDir);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path FileTreeDirToDotMeddyprojectDir(boost::filesystem::path&& fileTreeDir);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path MeddydataPathToMeddydataManifestPath(boost::filesystem::path&& meddydataPath);

    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path MeddydataManifestPathToMeddydataPath(boost::filesystem::path&& manifestMeddydataPath);

    enum class Result_QueryWhetherPathIsFileTreeDir : unsigned char
    {
        Yes,
        No_LeafNameDoesNotMatch,
        No_NotImmediateChildOfDotMeddyproject
    };

    MEDDYSDK_MEDDYDATA_EXPORT bool IsFileTreeDir(const boost::filesystem::path& filesystemPath);
    MEDDYSDK_MEDDYDATA_EXPORT Result_QueryWhetherPathIsFileTreeDir QueryWhetherPathIsFileTreeDir(const boost::filesystem::path& filesystemPath);

    enum class Result_QueryWhetherPathIsValidMeddydata : unsigned char
    {
        Yes,
        No_DoesntExist,
        No_NotDirectory,
        No_NotUnderMeddyprojectFileTreeDir
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

    template <class Encoding, class Allocator, class StackAllocator>
    void AppendNewMetadataJson(const MeddySDK::Meddydata& meddydata, rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>& metadataJsonDocument);
}
