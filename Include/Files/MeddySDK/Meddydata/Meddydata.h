// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddyproject_Export.h>
#include <filesystem>
#include <utility>

namespace MeddySDK
{
    enum MeddydataConstructor_FromSourcePath
    {
        FromSourcePath
    };

    enum MeddydataConstructor_FromMeddydataPath
    {
        FromMeddydataPath
    };

    struct MEDDYSDK_MEDDYPROJECT_EXPORT Meddydata
    {
    public:
        explicit Meddydata(MeddydataConstructor_FromSourcePath, std::filesystem::path&& sourceFilesystemPath);
        explicit Meddydata(MeddydataConstructor_FromMeddydataPath, std::filesystem::path&& meddydataFilesystemPath);

    public:
        const std::filesystem::path& GetSourceFilesystemPath() const &;
        std::filesystem::path GetSourceFilesystemPath() &&;
        std::filesystem::path GetMeddydataPath() const &;
        std::filesystem::path GetMeddydataPath() &&;

    private:
        std::filesystem::path SourceFilesystemPath;
    };
}
