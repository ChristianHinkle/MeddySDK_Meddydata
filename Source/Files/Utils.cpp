// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>
#include <MeddySDK/Meddydata/Utils.inl>

#include <filesystem>
#include <cstdlib>
#include <cassert>
#include <MeddySDK/Meddyproject/Utils.h>
#include <iostream>
#include <MeddySDK/Meddyproject/FilesystemUtils.h>
#include <rapidjson/document.h>
#include <MeddySDK/Meddyproject/Json.h>
#include <MeddySDK/Meddyproject/Json.inl>
#include <utility>

CppUtils::ExpectedResult<MeddySDK::Meddydata, MeddySDK::Error_GetMeddydata> MeddySDK::GetMeddydata(std::filesystem::path&& sourceFilesystemPath)
{
    CppUtils::ExpectedResult result = MeddySDK::GetPathToMeddydata(std::move(sourceFilesystemPath));

    if (result.IsError())
    {
        switch (result.GetError())
        {
        case Error_GetPathToMeddydata::SourcePathDoesNotExist:
            return Error_GetMeddydata::SourcePathDoesNotExist;
        case Error_GetPathToMeddydata::CouldntLocateOuterMeddyproject:
            return Error_GetMeddydata::CouldntLocateOuterMeddyproject;
        }

        assert(false); // Hits if there is an error returned that we don't have a case for in here.
        return Error_GetMeddydata{};
    }

    std::filesystem::path meddydataPath = std::move(result).GetValue();
    if (!std::filesystem::exists(meddydataPath))
    {
        return Error_GetMeddydata::NoMeddydata_DoesntExist;
    }

    if (!std::filesystem::is_directory(meddydataPath))
    {
        return Error_GetMeddydata::NoMeddydata_NotDirectory;
    }

    return MeddySDK::Meddydata{FromMeddydataPath, std::move(meddydataPath)};
}

CppUtils::ExpectedResult<std::filesystem::path, MeddySDK::Error_GetPathToMeddydata> MeddySDK::GetPathToMeddydata(
    std::filesystem::path&& sourceFilesystemPath)
{
    // TODO: Make version of this function which allows it not to exist.
    if (!std::filesystem::exists(sourceFilesystemPath))
    {
        return Error_GetPathToMeddydata::SourcePathDoesNotExist;
    }

    CppUtils::ExpectedResult outerMeddyprojectResult = MeddySDK::GetOuterMeddyproject(std::filesystem::path{sourceFilesystemPath});
    if (outerMeddyprojectResult.IsError())
    {
        return Error_GetPathToMeddydata::CouldntLocateOuterMeddyproject;
    }

    std::filesystem::path outerMeddyprojectRootDir = std::move(outerMeddyprojectResult).GetValue().GetRootPath();
    std::filesystem::path sourcePathRelativeToMeddyprojectRoot = sourceFilesystemPath.lexically_relative(outerMeddyprojectRootDir);

    return GetPathToMeddydata(std::move(outerMeddyprojectRootDir), std::move(sourcePathRelativeToMeddyprojectRoot));
}

std::filesystem::path MeddySDK::GetPathToMeddydata(
    std::filesystem::path&& meddyprojectRootPath, std::filesystem::path&& sourcePathRelative)
{
    if (!MeddySDK::IsValidProjectRoot(std::filesystem::path{meddyprojectRootPath}))
    {
        std::cerr << "Error: [" << __func__ << "] A non-meddyproject path was passed in!" << '\n';
        assert(false);
    }

    if (!sourcePathRelative.is_relative())
    {
        std::cerr << "Error: [" << __func__ << "] A non-relative path was passed in for the source file!" << '\n';
        assert(false);
    }

    std::filesystem::path meddyprojectDirPath = MeddySDK::ProjectRootToMeddyprojectDirPath(std::move(meddyprojectRootPath));

    std::filesystem::path fileTreeDirPath = MeddyprojectDirToFileTreeDir(std::move(meddyprojectDirPath));

    std::filesystem::path sourcePathRelativeToFileTree = std::move(fileTreeDirPath.operator/=(sourcePathRelative));

    return std::move(sourcePathRelativeToFileTree.append(MeddydataDirString));
}

std::filesystem::path MeddySDK::MeddyprojectDirToFileTreeDir(std::filesystem::path&& meddyprojectDir)
{
    return std::move(meddyprojectDir).append(FileTreeDirString);
}

std::filesystem::path MeddySDK::FileTreeDirToMeddyprojectDir(std::filesystem::path&& fileTreeDir)
{
    return std::move(fileTreeDir).parent_path();
}

std::filesystem::path MeddySDK::MeddydataPathToMeddydataManifestPath(std::filesystem::path&& meddydataPath)
{
    return std::move(meddydataPath).append(MeddydataManifestFilename);
}

std::filesystem::path MeddySDK::MeddydataManifestPathToMeddydataPath(std::filesystem::path&& manifestMeddydataPath)
{
    return std::move(manifestMeddydataPath).parent_path();
}

bool MeddySDK::IsFileTreeDir(const std::filesystem::path& filesystemPath)
{
    Result_QueryWhetherPathIsFileTreeDir result = MeddySDK::QueryWhetherPathIsFileTreeDir(filesystemPath);
    return result == Result_QueryWhetherPathIsFileTreeDir::Yes;
}

MeddySDK::Result_QueryWhetherPathIsFileTreeDir MeddySDK::QueryWhetherPathIsFileTreeDir(const std::filesystem::path& filesystemPath)
{
    if (!MeddySDK::IsPathEqualToString(filesystemPath.filename(), FileTreeDirString))
    {
        return Result_QueryWhetherPathIsFileTreeDir::No_LeafNameDoesNotMatch;
    }

    if (!MeddySDK::IsMeddyprojectDirPath(filesystemPath.parent_path()))
    {
        return Result_QueryWhetherPathIsFileTreeDir::No_NotImmediateChildOfMeddyprojectDir;
    }

    return Result_QueryWhetherPathIsFileTreeDir::Yes;
}

MeddySDK::Result_QueryWhetherPathIsValidMeddydata MeddySDK::QueryWhetherPathIsValidMeddydata(std::filesystem::path&& meddydataPath)
{
    if (!std::filesystem::exists(meddydataPath))
    {
        return Result_QueryWhetherPathIsValidMeddydata::No_DoesntExist;
    }

    if (!std::filesystem::is_directory(meddydataPath))
    {
        return Result_QueryWhetherPathIsValidMeddydata::No_NotDirectory;
    }

    // Traverse up the parent directories until we see that "_meddyproject/ftree" exists.
    // TODO: [todo][techdebt] Fix loop condition to also allow the iteration on the root directory if it gets to that point.
    for (std::filesystem::path currentDir = std::move(meddydataPath); !CppUtils::IsRootPath(currentDir); currentDir = std::move(currentDir).parent_path())
    {
        if (IsFileTreeDir(currentDir))
        {
            return Result_QueryWhetherPathIsValidMeddydata::Yes;
        }
    }

    return Result_QueryWhetherPathIsValidMeddydata::No_NotUnderMeddyprojectFileTreeDir;
}

CppUtils::ExpectedResult<std::filesystem::path, MeddySDK::Result_QueryWhetherPathIsValidMeddydata> MeddySDK::GetSourceFilePathFromMeddydataPath(
    const std::filesystem::path& meddydataPath)
{
    Result_QueryWhetherPathIsValidMeddydata isValidMeddydataResult = QueryWhetherPathIsValidMeddydata(std::filesystem::path{meddydataPath});
    if (isValidMeddydataResult != Result_QueryWhetherPathIsValidMeddydata::Yes)
    {
        // Error, not a valid meddydata path.
        return isValidMeddydataResult;
    }

    // Traverse up the parent directories until we see that "_meddyproject/ftree" exists.
    // TODO: [todo][techdebt] Fix loop condition to also allow the iteration on the root directory if it gets to that point.
    for (std::filesystem::path currentDir = meddydataPath; !CppUtils::IsRootPath(currentDir); currentDir = std::move(currentDir).parent_path())
    {
        if (IsFileTreeDir(currentDir))
        {
            // We know that the meddydata path's leaf is "_meddydata". Get remove that part of it, and make it relative to the meddyproject's root dir.
            assert(MeddySDK::IsPathEqualToString(meddydataPath.filename(), MeddydataDirString));
            std::filesystem::path sourceFilePathRelative = meddydataPath.parent_path().lexically_relative(currentDir);

            std::filesystem::path meddyprojectDir = FileTreeDirToMeddyprojectDir(std::move(currentDir));
            std::filesystem::path meddyprojectRootDir = MeddySDK::MeddyprojectDirToProjectRootPath(std::move(meddyprojectDir));

            return std::move(meddyprojectRootDir.operator/=(sourceFilePathRelative));
        }
    }

    assert(false); // This case is logically unreachable.
    return Result_QueryWhetherPathIsValidMeddydata{};
}

CppUtils::ExpectedResult<MeddySDK::Meddydata, MeddySDK::Error_TryAddMeddydata> MeddySDK::TryAddMeddydata(std::filesystem::path&& sourceFilesystemPath)
{
    CppUtils::ExpectedResult meddyprojectResult = MeddySDK::GetOuterMeddyproject(std::filesystem::path{sourceFilesystemPath});
    if (meddyprojectResult.IsError())
    {
        switch (meddyprojectResult.GetError())
        {
        case MeddySDK::Error_GetOuterMeddyprojectDirPath::PathDoesntExist:
            return Error_TryAddMeddydata::SourcePathDoesNotExist;
        case MeddySDK::Error_GetOuterMeddyprojectDirPath::NoMeddyprojectDirFound:
            return Error_TryAddMeddydata::CouldntLocateOuterMeddyproject;
        }

        assert(false); // Hits if there is an error returned that we don't have a case for in here.
        return Error_TryAddMeddydata{};
    }

    MeddySDK::Meddyproject meddyproject = std::move(meddyprojectResult).GetValue();

    std::filesystem::path pathToSourceFileRelative = sourceFilesystemPath.lexically_relative(meddyproject.GetRootPath());

    CppUtils::ExpectedResult meddydataResult = AddMeddydata(std::move(meddyproject), std::move(pathToSourceFileRelative));
    if (meddydataResult.IsError())
    {
        switch (meddydataResult.GetError())
        {
        case MeddySDK::Error_AddMeddydata::SourceFilePathContainsReservedMeddydataDirName:
            return MeddySDK::Error_TryAddMeddydata::SourceFilePathContainsReservedMeddydataDirName;
        case MeddySDK::Error_AddMeddydata::FilesystemFailedToCreateMeddydata:
            return Error_TryAddMeddydata::FilesystemFailedToCreateMeddydata;
        case MeddySDK::Error_AddMeddydata::FilesystemFailedToCreateManifestFile:
            return Error_TryAddMeddydata::FilesystemFailedToCreateManifestFile;
        }

        assert(false); // Hits if there is an error returned that we don't have a case for in here.
        return Error_TryAddMeddydata{};
    }

    return std::move(meddydataResult).GetValue();
}

CppUtils::ExpectedResult<MeddySDK::Meddydata, MeddySDK::Error_AddMeddydata> MeddySDK::AddMeddydata(
    MeddySDK::Meddyproject&& meddyproject,
    std::filesystem::path&& sourcePathRelative)
{
    // Ensure that the user's source file path does not contain our reserved meddydata dir name anywhere in it, as that would cause problems or be
    // a very complex scenario to support.
    for (std::filesystem::path currentPath = sourcePathRelative; !currentPath.empty(); currentPath = currentPath.has_parent_path() ? std::move(currentPath).parent_path() : std::filesystem::path{})
    {
        if (MeddySDK::IsPathEqualToString(currentPath.filename(), MeddydataDirString))
        {
            return Error_AddMeddydata::SourceFilePathContainsReservedMeddydataDirName;
        }
    }

    std::filesystem::path pathToMeddydata = GetPathToMeddydata(std::move(meddyproject).GetRootPath(), std::move(sourcePathRelative));

    if (std::filesystem::exists(pathToMeddydata) && !std::filesystem::is_directory(pathToMeddydata))
    {
        // There is a file conflicting with the same name as the directory we wanted to create.
        return Error_AddMeddydata::FilesystemFailedToCreateMeddydata;
    }

    std::filesystem::path manifestFilePath = MeddydataPathToMeddydataManifestPath(std::move(pathToMeddydata));

    if (std::filesystem::exists(manifestFilePath) && std::filesystem::is_directory(manifestFilePath))
    {
        // Return an error if there was a directory conflicting with the file name that we wanted to create.
        return Error_AddMeddydata::FilesystemFailedToCreateManifestFile;
    }

    pathToMeddydata = MeddydataManifestPathToMeddydataPath(std::move(manifestFilePath));

    // No seriously conflicting files, proceed with creating the meddydata files.

    if (!std::filesystem::exists(pathToMeddydata))
    {
        const bool didCreateMeddydata = std::filesystem::create_directories(pathToMeddydata);
        if (!didCreateMeddydata)
        {
            // Note: This could happen if a file exists with a conflicting name with one of the directories we're trying to create.
            return Error_AddMeddydata::FilesystemFailedToCreateMeddydata;
        }
    }

    manifestFilePath = MeddydataPathToMeddydataManifestPath(std::move(pathToMeddydata));

    // If it already exists, just use it without touching its contents.
    if (std::filesystem::exists(manifestFilePath))
    {
        return MeddySDK::Meddydata{FromMeddydataPath, MeddydataManifestPathToMeddydataPath(std::move(manifestFilePath))};
    }

    // Create the file.
    const bool didCreateFile = CppUtils::TouchNewFile(manifestFilePath.native());
    if (!didCreateFile)
    {
        return Error_AddMeddydata::FilesystemFailedToCreateManifestFile;
    }

    MeddySDK::Meddydata meddydata{FromMeddydataPath, MeddydataManifestPathToMeddydataPath(std::filesystem::path{manifestFilePath})};

    // Populate the json file.
    {
        // Write the default json contents.
        rapidjson::Document jsonDocument{};
        jsonDocument.SetObject();
        AppendNewMetadataJson(meddydata, jsonDocument);

        // Serialize this json document to the file.
        const bool didSaveToFile = MeddySDK::SaveJsonDocumentToFile(CppUtils::GetStringViewFromPath(manifestFilePath), jsonDocument);
        assert(didSaveToFile);
    }

    return meddydata;
}
