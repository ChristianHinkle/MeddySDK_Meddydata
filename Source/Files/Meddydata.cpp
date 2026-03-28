// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Meddydata.h>

#include <MeddySDK/Meddydata/Utils.h>
#include <boost/filesystem/path.hpp>
#include <utility>
#include <cassert>
#include <iostream>

MeddySDK::Meddydata::Meddydata(MeddydataConstructor_FromSourcePath, boost::filesystem::path&& sourceFilesystemPath)
    : SourceFilesystemPath{std::move(sourceFilesystemPath)}
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(boost::filesystem::path{SourceFilesystemPath});
    if (meddydataPathResult.IsError())
    {
        std::cerr << "Error: " << "Tried constructing a meddydata struct from a path with no meddydata!" << '\n';
        assert(false);
    }
}

MeddySDK::Meddydata::Meddydata(MeddydataConstructor_FromMeddydataPath, boost::filesystem::path&& meddydataFilesystemPath)
{
    CppUtils::ExpectedResult sourceFilePathResult = MeddySDK::GetSourceFilePathFromMeddydataPath(std::move(meddydataFilesystemPath));
    if (sourceFilePathResult.IsError())
    {
        std::cerr << "Error: " << "Tried constructing a meddydata struct from an invalid meddydata path!" << '\n';
        assert(false);
    }

    SourceFilesystemPath = std::move(sourceFilePathResult).GetValue();
}

const boost::filesystem::path& MeddySDK::Meddydata::GetSourceFilesystemPath() const &
{
    return SourceFilesystemPath;
}
boost::filesystem::path MeddySDK::Meddydata::GetSourceFilesystemPath() &&
{
    boost::filesystem::path& result = const_cast<boost::filesystem::path&>(GetSourceFilesystemPath());
    return std::move(result);
}

boost::filesystem::path MeddySDK::Meddydata::GetMeddydataPath() const &
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(boost::filesystem::path{SourceFilesystemPath});
    assert(!meddydataPathResult.IsError()); // We already verified this fact when constructing.
    return std::move(meddydataPathResult).GetValue();
}
boost::filesystem::path MeddySDK::Meddydata::GetMeddydataPath() &&
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(std::move(SourceFilesystemPath));
    assert(!meddydataPathResult.IsError()); // We already verified this fact when constructing.
    return std::move(meddydataPathResult).GetValue();
}
