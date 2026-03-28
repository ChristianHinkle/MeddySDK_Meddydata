// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <cassert>
#include <MeddySDK/Meddyproject/Utils.h>
#include <iostream>
#include <MeddySDK/Meddyproject/FilesystemUtils.h>
#include <CppUtils/Core/Filesystem.h>

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

    boost::filesystem::path dotMeddyprojectPath = MeddySDK::ProjectRootToDotMeddyprojectPath(std::move(meddyprojectRootPath));

    boost::filesystem::path meddydataRootDirPath = DotMeddyprojectDirToMeddydataRootDir(std::move(dotMeddyprojectPath));

    return std::move(meddydataRootDirPath).append(std::move(sourcePathRelative));
}

boost::filesystem::path MeddySDK::DotMeddyprojectDirToMeddydataRootDir(boost::filesystem::path&& dotMeddyprojectDir)
{
    return std::move(dotMeddyprojectDir).append(MeddydataRootDirString);
}

boost::filesystem::path MeddySDK::MeddydataRootDirToDotMeddyprojectDir(boost::filesystem::path&& meddydataRootDir)
{
    return std::move(meddydataRootDir).parent_path();
}

boost::filesystem::path MeddySDK::MeddydataPathToMeddydataManifestPath(boost::filesystem::path&& meddydataPath)
{
    return std::move(meddydataPath).append(MeddydataManifestFilename);
}

boost::filesystem::path MeddySDK::MeddydataManifestPathToMeddydataPath(boost::filesystem::path&& manifestMeddydataPath)
{
    return std::move(manifestMeddydataPath).parent_path();
}

bool MeddySDK::IsMeddydataRootDir(const boost::filesystem::path& filesystemPath)
{
    Result_QueryWhetherPathIsMeddydataRootDir result = MeddySDK::QueryWhetherPathIsMeddydataRootDir(filesystemPath);
    return result == Result_QueryWhetherPathIsMeddydataRootDir::Yes;
}

MeddySDK::Result_QueryWhetherPathIsMeddydataRootDir MeddySDK::QueryWhetherPathIsMeddydataRootDir(const boost::filesystem::path& filesystemPath)
{
    if (!MeddySDK::IsPathEqualToString(filesystemPath.filename(), MeddydataRootDirString))
    {
        return Result_QueryWhetherPathIsMeddydataRootDir::No_LeafNameIsNotEqualToMeddydata;
    }

    if (!MeddySDK::IsDotMeddyprojectPath(filesystemPath.parent_path()))
    {
        return Result_QueryWhetherPathIsMeddydataRootDir::No_NotImmediateChildOfDotMeddyproject;
    }

    return Result_QueryWhetherPathIsMeddydataRootDir::Yes;
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

    // Traverse up the parent directories until we see that ".meddyproject/meddydata" exists.
    for (boost::filesystem::path currentDir = std::move(meddydataPath); currentDir.has_parent_path(); currentDir = std::move(currentDir).parent_path())
    {
        if (IsMeddydataRootDir(currentDir))
        {
            return Result_QueryWhetherPathIsValidMeddydata::Yes;
        }
    }

    return Result_QueryWhetherPathIsValidMeddydata::No_NotUnderMeddyprojectMeddydataRootDir;
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

    // Traverse up the parent directories until we see that ".meddyproject/meddydata" exists.
    for (boost::filesystem::path currentDir = meddydataPath; currentDir.has_parent_path(); currentDir = std::move(currentDir).parent_path())
    {
        if (IsMeddydataRootDir(currentDir))
        {
            boost::filesystem::path sourceFilePathRelative = meddydataPath.lexically_relative(currentDir);

            boost::filesystem::path dotMeddyprojectDir = MeddydataRootDirToDotMeddyprojectDir(std::move(currentDir));
            boost::filesystem::path meddyprojectRootDir = MeddySDK::DotMeddyprojectToProjectRootPath(std::move(dotMeddyprojectDir));

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
        case MeddySDK::Error_GetOuterDotMeddyprojectPath::PathDoesntExist:
            return Error_TryAddMeddydata::SourcePathDoesNotExist;
        case MeddySDK::Error_GetOuterDotMeddyprojectPath::NoDotMeddyprojectFound:
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

    // TODO: Handle specifically case where directory already exists.
    const bool didCreateMeddydata = boost::filesystem::create_directories(pathToMeddydata);
    if (!didCreateMeddydata)
    {
        return Error_AddMeddydata::FilesystemFailedToCreateMeddydata;
    }

    // TODO: Handle specifically case where manifest file already exists.
    boost::filesystem::path manifestFilePath = MeddydataPathToMeddydataManifestPath(std::move(pathToMeddydata));

    const bool didCreateFile = CppUtils::TouchNewFile(manifestFilePath.native());
    if (!didCreateFile)
    {
        return Error_AddMeddydata::FilesystemFailedToCreateManifestFile;
    }

    return MeddySDK::Meddydata{FromMeddydataPath, MeddydataManifestPathToMeddydataPath(std::move(manifestFilePath))};
}
