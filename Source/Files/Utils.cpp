// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>
#include <MeddySDK/Meddydata/Utils.inl>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <cassert>
#include <MeddySDK/Meddyproject/Utils.h>
#include <iostream>
#include <MeddySDK/Meddyproject/FilesystemUtils.h>
#include <rapidjson/document.h>
#include <MeddySDK/Meddyproject/Json.h>
#include <MeddySDK/Meddyproject/Json.inl>

CppUtils::ExpectedResult<MeddySDK::Meddydata, MeddySDK::Error_GetMeddydata> MeddySDK::GetMeddydata(boost::filesystem::path&& sourceFilesystemPath)
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

    boost::filesystem::path meddydataPath = std::move(result).GetValue();
    if (!boost::filesystem::exists(meddydataPath))
    {
        return Error_GetMeddydata::NoMeddydata_DoesntExist;
    }

    if (!boost::filesystem::is_directory(meddydataPath))
    {
        return Error_GetMeddydata::NoMeddydata_NotDirectory;
    }

    return MeddySDK::Meddydata{FromMeddydataPath, std::move(meddydataPath)};
}

CppUtils::ExpectedResult<boost::filesystem::path, MeddySDK::Error_GetPathToMeddydata> MeddySDK::GetPathToMeddydata(
    boost::filesystem::path&& sourceFilesystemPath)
{
    // TODO: Make version of this function which allows it not to exist.
    if (!boost::filesystem::exists(sourceFilesystemPath))
    {
        return Error_GetPathToMeddydata::SourcePathDoesNotExist;
    }

    CppUtils::ExpectedResult outerMeddyprojectResult = MeddySDK::GetOuterMeddyproject(boost::filesystem::path{sourceFilesystemPath});
    if (outerMeddyprojectResult.IsError())
    {
        return Error_GetPathToMeddydata::CouldntLocateOuterMeddyproject;
    }

    boost::filesystem::path outerMeddyprojectRootDir = std::move(outerMeddyprojectResult).GetValue().GetRootPath();
    boost::filesystem::path sourcePathRelativeToMeddyprojectRoot = sourceFilesystemPath.lexically_relative(outerMeddyprojectRootDir);

    return GetPathToMeddydata(std::move(outerMeddyprojectRootDir), std::move(sourcePathRelativeToMeddyprojectRoot));
}

boost::filesystem::path MeddySDK::GetPathToMeddydata(
    boost::filesystem::path&& meddyprojectRootPath, boost::filesystem::path&& sourcePathRelative)
{
    if (!MeddySDK::IsValidProjectRoot(boost::filesystem::path{meddyprojectRootPath}))
    {
        std::cerr << "Error: [" << __func__ << "] A non-meddyproject path was passed in!" << '\n';
        assert(false);
    }

    if (!sourcePathRelative.is_relative())
    {
        std::cerr << "Error: [" << __func__ << "] A non-relative path was passed in for the source file!" << '\n';
        assert(false);
    }

    boost::filesystem::path meddyprojectDirPath = MeddySDK::ProjectRootToMeddyprojectDirPath(std::move(meddyprojectRootPath));

    boost::filesystem::path fileTreeDirPath = MeddyprojectDirToFileTreeDir(std::move(meddyprojectDirPath));

    boost::filesystem::path sourcePathRelativeToFileTree = std::move(fileTreeDirPath.append(std::move(sourcePathRelative)));

    return std::move(sourcePathRelativeToFileTree.append(MeddydataDirString));
}

boost::filesystem::path MeddySDK::MeddyprojectDirToFileTreeDir(boost::filesystem::path&& meddyprojectDir)
{
    return std::move(meddyprojectDir).append(FileTreeDirString);
}

boost::filesystem::path MeddySDK::FileTreeDirToMeddyprojectDir(boost::filesystem::path&& fileTreeDir)
{
    return std::move(fileTreeDir).parent_path();
}

boost::filesystem::path MeddySDK::MeddydataPathToMeddydataManifestPath(boost::filesystem::path&& meddydataPath)
{
    return std::move(meddydataPath).append(MeddydataManifestFilename);
}

boost::filesystem::path MeddySDK::MeddydataManifestPathToMeddydataPath(boost::filesystem::path&& manifestMeddydataPath)
{
    return std::move(manifestMeddydataPath).parent_path();
}

bool MeddySDK::IsFileTreeDir(const boost::filesystem::path& filesystemPath)
{
    Result_QueryWhetherPathIsFileTreeDir result = MeddySDK::QueryWhetherPathIsFileTreeDir(filesystemPath);
    return result == Result_QueryWhetherPathIsFileTreeDir::Yes;
}

MeddySDK::Result_QueryWhetherPathIsFileTreeDir MeddySDK::QueryWhetherPathIsFileTreeDir(const boost::filesystem::path& filesystemPath)
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

MeddySDK::Result_QueryWhetherPathIsValidMeddydata MeddySDK::QueryWhetherPathIsValidMeddydata(boost::filesystem::path&& meddydataPath)
{
    if (!boost::filesystem::exists(meddydataPath))
    {
        return Result_QueryWhetherPathIsValidMeddydata::No_DoesntExist;
    }

    if (!boost::filesystem::is_directory(meddydataPath))
    {
        return Result_QueryWhetherPathIsValidMeddydata::No_NotDirectory;
    }

    // Traverse up the parent directories until we see that "_meddyproject/ftree" exists.
    for (boost::filesystem::path currentDir = std::move(meddydataPath); currentDir.has_parent_path(); currentDir = std::move(currentDir).parent_path())
    {
        if (IsFileTreeDir(currentDir))
        {
            return Result_QueryWhetherPathIsValidMeddydata::Yes;
        }
    }

    return Result_QueryWhetherPathIsValidMeddydata::No_NotUnderMeddyprojectFileTreeDir;
}

CppUtils::ExpectedResult<boost::filesystem::path, MeddySDK::Result_QueryWhetherPathIsValidMeddydata> MeddySDK::GetSourceFilePathFromMeddydataPath(
    const boost::filesystem::path& meddydataPath)
{
    Result_QueryWhetherPathIsValidMeddydata isValidMeddydataResult = QueryWhetherPathIsValidMeddydata(boost::filesystem::path{meddydataPath});
    if (isValidMeddydataResult != Result_QueryWhetherPathIsValidMeddydata::Yes)
    {
        // Error, not a valid meddydata path.
        return isValidMeddydataResult;
    }

    // Traverse up the parent directories until we see that "_meddyproject/ftree" exists.
    for (boost::filesystem::path currentDir = meddydataPath; currentDir.has_parent_path(); currentDir = std::move(currentDir).parent_path())
    {
        if (IsFileTreeDir(currentDir))
        {
            // We know that the meddydata path's leaf is "_meddydata". Get remove that part of it, and make it relative to the meddyproject's root dir.
            assert(MeddySDK::IsPathEqualToString(meddydataPath.filename(), MeddydataDirString));
            boost::filesystem::path sourceFilePathRelative = meddydataPath.parent_path().lexically_relative(currentDir);

            boost::filesystem::path meddyprojectDir = FileTreeDirToMeddyprojectDir(std::move(currentDir));
            boost::filesystem::path meddyprojectRootDir = MeddySDK::MeddyprojectDirToProjectRootPath(std::move(meddyprojectDir));

            return std::move(std::move(meddyprojectRootDir).append(sourceFilePathRelative));
        }
    }

    assert(false); // This case is logically unreachable.
    return Result_QueryWhetherPathIsValidMeddydata{};
}

CppUtils::ExpectedResult<MeddySDK::Meddydata, MeddySDK::Error_TryAddMeddydata> MeddySDK::TryAddMeddydata(boost::filesystem::path&& sourceFilesystemPath)
{
    CppUtils::ExpectedResult meddyprojectResult = MeddySDK::GetOuterMeddyproject(boost::filesystem::path{sourceFilesystemPath});
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

    boost::filesystem::path pathToSourceFileRelative = sourceFilesystemPath.lexically_relative(meddyproject.GetRootPath());

    CppUtils::ExpectedResult meddydataResult = AddMeddydata(std::move(meddyproject), std::move(pathToSourceFileRelative));
    if (meddydataResult.IsError())
    {
        switch (meddydataResult.GetError())
        {
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
    boost::filesystem::path&& sourcePathRelative)
{
    boost::filesystem::path pathToMeddydata = GetPathToMeddydata(std::move(meddyproject).GetRootPath(), std::move(sourcePathRelative));

    if (boost::filesystem::exists(pathToMeddydata) && !boost::filesystem::is_directory(pathToMeddydata))
    {
        // There is a file conflicting with the same name as the directory we wanted to create. TODO: We should traverse up the parent directories and check for conflicting files.
        return Error_AddMeddydata::FilesystemFailedToCreateMeddydata;
    }

    boost::filesystem::path manifestFilePath = MeddydataPathToMeddydataManifestPath(std::move(pathToMeddydata));

    if (boost::filesystem::exists(manifestFilePath) && boost::filesystem::is_directory(manifestFilePath))
    {
        // Return an error if there was a directory conflicting with the file name that we wanted to create.
        return Error_AddMeddydata::FilesystemFailedToCreateManifestFile;
    }

    pathToMeddydata = MeddydataManifestPathToMeddydataPath(std::move(manifestFilePath));

    // No seriously conflicting files, proceed with creating the meddydata files.

    if (!boost::filesystem::exists(pathToMeddydata))
    {
        const bool didCreateMeddydata = boost::filesystem::create_directories(pathToMeddydata);
        if (!didCreateMeddydata)
        {
            // Note: This could happen if a file exists with a conflicting name with one of the directories we're trying to create.
            return Error_AddMeddydata::FilesystemFailedToCreateMeddydata;
        }
    }

    manifestFilePath = MeddydataPathToMeddydataManifestPath(std::move(pathToMeddydata));

    // If it already exists, just use it without touching its contents.
    if (boost::filesystem::exists(manifestFilePath))
    {
        return MeddySDK::Meddydata{FromMeddydataPath, MeddydataManifestPathToMeddydataPath(std::move(manifestFilePath))};
    }

    // Create the file.
    const bool didCreateFile = CppUtils::TouchNewFile(manifestFilePath.native());
    if (!didCreateFile)
    {
        return Error_AddMeddydata::FilesystemFailedToCreateManifestFile;
    }

    MeddySDK::Meddydata meddydata{FromMeddydataPath, MeddydataManifestPathToMeddydataPath(boost::filesystem::path{manifestFilePath})};

    // Populate the json file.
    {
        // Write the default json contents.
        rapidjson::Document jsonDocument{};
        jsonDocument.SetObject();
        AppendNewMetadataJson(meddydata, jsonDocument);

        // Serialize this json document to the file.
        const bool didSaveToFile = MeddySDK::SaveJsonDocumentToFile(CppUtils::StdPathStringView{manifestFilePath.native()}, jsonDocument);
        assert(didSaveToFile);
    }

    return meddydata;
}
