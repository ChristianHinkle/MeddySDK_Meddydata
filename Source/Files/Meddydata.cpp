// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#include <MeddySDK/Meddydata/Meddydata.h>

#include <MeddySDK/Meddydata/Utils.h>
#include <filesystem>
#include <utility>
#include <cassert>
#include <iostream>

MeddySDK::Meddydata::Meddydata(MeddydataConstructor_FromSourcePath, std::filesystem::path&& sourceFilesystemPath)
    : SourceFilesystemPath{std::move(sourceFilesystemPath)}
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(std::filesystem::path{SourceFilesystemPath});
    if (meddydataPathResult.IsError())
    {
        std::cerr << "Error: " << "Tried constructing a meddydata struct from a path with no meddydata!" << '\n';
        assert(false);
    }
}

MeddySDK::Meddydata::Meddydata(MeddydataConstructor_FromMeddydataPath, std::filesystem::path&& meddydataFilesystemPath)
{
    CppUtils::ExpectedResult sourceFilePathResult = MeddySDK::GetSourceFilePathFromMeddydataPath(std::move(meddydataFilesystemPath));
    if (sourceFilePathResult.IsError())
    {
        std::cerr << "Error: " << "Tried constructing a meddydata struct from an invalid meddydata path!" << '\n';
        assert(false);
    }

    SourceFilesystemPath = std::move(sourceFilePathResult).GetValue();
}

const std::filesystem::path& MeddySDK::Meddydata::GetSourceFilesystemPath() const &
{
    return SourceFilesystemPath;
}
std::filesystem::path MeddySDK::Meddydata::GetSourceFilesystemPath() &&
{
    std::filesystem::path& result = const_cast<std::filesystem::path&>(GetSourceFilesystemPath());
    return std::move(result);
}

std::filesystem::path MeddySDK::Meddydata::GetMeddydataPath() const &
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(std::filesystem::path{SourceFilesystemPath});
    assert(!meddydataPathResult.IsError()); // We already verified this fact when constructing.
    return std::move(meddydataPathResult).GetValue();
}
std::filesystem::path MeddySDK::Meddydata::GetMeddydataPath() &&
{
    CppUtils::ExpectedResult meddydataPathResult = MeddySDK::GetPathToMeddydata(std::move(SourceFilesystemPath));
    assert(!meddydataPathResult.IsError()); // We already verified this fact when constructing.
    return std::move(meddydataPathResult).GetValue();
}
