// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddyproject_Export.h>
#include <boost/filesystem/path.hpp>
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
        explicit Meddydata(MeddydataConstructor_FromSourcePath, boost::filesystem::path&& sourceFilesystemPath);
        explicit Meddydata(MeddydataConstructor_FromMeddydataPath, boost::filesystem::path&& meddydataFilesystemPath);

    public:
        const boost::filesystem::path& GetSourceFilesystemPath() const &;
        boost::filesystem::path GetSourceFilesystemPath() &&;
        boost::filesystem::path GetMeddydataPath() const &;
        boost::filesystem::path GetMeddydataPath() &&;

    private:
        boost::filesystem::path SourceFilesystemPath;
    };
}
