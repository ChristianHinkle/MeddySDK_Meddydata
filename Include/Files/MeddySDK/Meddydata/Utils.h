// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK_Meddydata_Export.h>
#include <boost/filesystem/path.hpp>

/**
 *
 */
namespace MeddySDK
{
    MEDDYSDK_MEDDYDATA_EXPORT boost::filesystem::path GetPathToMeddydata(
        boost::filesystem::path&& sourceFilePath);
}
