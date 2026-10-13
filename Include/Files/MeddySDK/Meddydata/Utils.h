// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddydata_Export.h>
#include <filesystem>
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

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_GetMeddydata> GetMeddydata(std::filesystem::path&& sourceFilesystemPath);

    enum class Error_GetPathToMeddydata : unsigned char
    {
        SourcePathDoesNotExist,
        CouldntLocateOuterMeddyproject
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<std::filesystem::path, Error_GetPathToMeddydata> GetPathToMeddydata(
        std::filesystem::path&& sourceFilesystemPath);

    MEDDYSDK_MEDDYDATA_EXPORT std::filesystem::path GetPathToMeddydata(
        std::filesystem::path&& meddyprojectRootPath, std::filesystem::path&& sourcePathRelative);

    MEDDYSDK_MEDDYDATA_EXPORT std::filesystem::path MeddyprojectDirToFileTreeDir(std::filesystem::path&& meddyprojectDir);

    MEDDYSDK_MEDDYDATA_EXPORT std::filesystem::path FileTreeDirToMeddyprojectDir(std::filesystem::path&& fileTreeDir);

    MEDDYSDK_MEDDYDATA_EXPORT std::filesystem::path MeddydataPathToMeddydataManifestPath(std::filesystem::path&& meddydataPath);

    MEDDYSDK_MEDDYDATA_EXPORT std::filesystem::path MeddydataManifestPathToMeddydataPath(std::filesystem::path&& manifestMeddydataPath);

    enum class Result_QueryWhetherPathIsFileTreeDir : unsigned char
    {
        Yes,
        No_LeafNameDoesNotMatch,
        No_NotImmediateChildOfMeddyprojectDir
    };

    MEDDYSDK_MEDDYDATA_EXPORT bool IsFileTreeDir(const std::filesystem::path& filesystemPath);
    MEDDYSDK_MEDDYDATA_EXPORT Result_QueryWhetherPathIsFileTreeDir QueryWhetherPathIsFileTreeDir(const std::filesystem::path& filesystemPath);

    enum class Result_QueryWhetherPathIsValidMeddydata : unsigned char
    {
        Yes,
        No_DoesntExist,
        No_NotDirectory,
        No_NotUnderMeddyprojectFileTreeDir
    };

    MEDDYSDK_MEDDYDATA_EXPORT Result_QueryWhetherPathIsValidMeddydata QueryWhetherPathIsValidMeddydata(std::filesystem::path&& meddydataPath);

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<std::filesystem::path, Result_QueryWhetherPathIsValidMeddydata> GetSourceFilePathFromMeddydataPath(
        const std::filesystem::path& meddydataPath);

    enum class Error_TryAddMeddydata : unsigned char
    {
        SourcePathDoesNotExist,
        CouldntLocateOuterMeddyproject,
        SourceFilePathContainsReservedMeddydataDirName,
        FilesystemFailedToCreateMeddydata,
        FilesystemFailedToCreateManifestFile,
    };

    // @Christian: TODO: [todo] Delete this "try" version of the function. It is unused and the new logic for it is handled in MeddyCLI. TODO: Migrate that
    // code to a new library specifically for handling SDK interaction and user-facing messages.
    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_TryAddMeddydata> TryAddMeddydata(std::filesystem::path&& sourceFilesystemPath);

    enum class Error_AddMeddydata : unsigned char
    {
        SourceFilePathContainsReservedMeddydataDirName,
        FilesystemFailedToCreateMeddydata,
        FilesystemFailedToCreateManifestFile,
    };

    MEDDYSDK_MEDDYDATA_EXPORT CppUtils::ExpectedResult<MeddySDK::Meddydata, Error_AddMeddydata> AddMeddydata(
        MeddySDK::Meddyproject&& meddyproject,
        std::filesystem::path&& sourcePathRelative);

    template <class Encoding, class Allocator, class StackAllocator>
    void AppendNewMetadataJson(const MeddySDK::Meddydata& meddydata, rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>& metadataJsonDocument);
}
