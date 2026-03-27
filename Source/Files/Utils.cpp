// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Utils.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <cassert>
#include <MeddySDK/Meddyproject/Utils.h>

CppUtils::ExpectedResult<boost::filesystem::path, MeddySDK::Error_GetPathToMeddydata> MeddySDK::GetPathToMeddydata(
    boost::filesystem::path&& sourceFilesystemPath)
{
    if (!boost::filesystem::exists(sourceFilesystemPath))
    {
        return Error_GetPathToMeddydata::SourceFilesystemPathDoesNotExist;
    }

    boost::filesystem::path correspondingMeddydataPath{};

    {
        CppUtils::ExpectedResult outerMeddyprojectResult = MeddySDK::GetOuterMeddyproject(boost::filesystem::path{sourceFilesystemPath});
        if (outerMeddyprojectResult.IsError())
        {
            return Error_GetPathToMeddydata::CouldntLocateOuterMeddyproject;
        }

        MeddySDK::Meddyproject outerMeddyproject = std::move(outerMeddyprojectResult).GetValue();

        boost::filesystem::path sourcePathRelativeToMeddyprojectRoot = sourceFilesystemPath.lexically_relative(outerMeddyproject.GetRootPath());

        boost::filesystem::path dotMeddyprojectPath = MeddySDK::ProjectRootToDotMeddyprojectPath(std::move(outerMeddyproject).GetRootPath());
        boost::filesystem::path meddydataDirectoryPath = std::move(dotMeddyprojectPath).append(MeddydataDirString);

        correspondingMeddydataPath = meddydataDirectoryPath.append(std::move(sourcePathRelativeToMeddyprojectRoot));
    }

    return correspondingMeddydataPath;
}
