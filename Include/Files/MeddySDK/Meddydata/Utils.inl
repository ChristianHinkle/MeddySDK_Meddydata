// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK/Meddydata/Utils.h>

#include <rapidjson/document.h>
#include <utility>
#include <MeddySDK/Meddydata/Meddydata.h>
#include <boost/filesystem/operations.hpp>
#include <format>
#include <chrono>
#include <CppUtils/Misc/DateTime.h>

template <class Encoding, class Allocator, class StackAllocator>
void MeddySDK::AppendNewMetadataJson(const MeddySDK::Meddydata& meddydata, rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>& metadataJsonDocument)
{
    assert(metadataJsonDocument.IsObject()); // The root must be an object.

    boost::filesystem::path sourcePathname = meddydata.GetSourceFilesystemPath();

    // @Christian: TODO: [todo][dam] This DCMI metadata should probably be handled by the DAM library.

    rapidjson::Value dcmiJsonObject{};
    dcmiJsonObject.SetObject();

    rapidjson::Value dcmiDateJsonObject{};
    dcmiDateJsonObject.SetObject();

    rapidjson::Value dcmiDateCreatedJsonString{};

    {
        const std::time_t creationTime = boost::filesystem::creation_time(sourcePathname);

        // Convert the creation timestamp to a chrono time relative to the local system's current UTC offset.
        std::chrono::zoned_time creationTimeZoned{
            std::chrono::current_zone(),
            std::chrono::system_clock::from_time_t(creationTime)
        };

        // TODO: Avoid the unnecessary string allocation via a custom char buffer along with `std::format_to_n`.
        auto creationTimeString = std::format(CPPUTILS_FORMAT_TIME_ISO_FULL_STRING_LITERAL, creationTimeZoned);

        dcmiDateCreatedJsonString.SetString(creationTimeString.data(), creationTimeString.length(), metadataJsonDocument.GetAllocator());
    }

    // See: https://dublincore.org/specifications/dublin-core/dcmi-terms/terms/created/.
    dcmiDateJsonObject.AddMember("Created", std::move(dcmiDateCreatedJsonString), metadataJsonDocument.GetAllocator());

    dcmiJsonObject.AddMember("Date", std::move(dcmiDateJsonObject), metadataJsonDocument.GetAllocator());

    metadataJsonDocument.AddMember("DCMI", std::move(dcmiJsonObject), metadataJsonDocument.GetAllocator());
}
