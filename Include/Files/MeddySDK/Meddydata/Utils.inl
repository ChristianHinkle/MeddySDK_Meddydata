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
        const std::chrono::zoned_time dateCreatedZonedTime = [&]()
        {
            const std::time_t creationTime = boost::filesystem::creation_time(sourcePathname);
            std::chrono::time_point creationTimePoint = std::chrono::system_clock::from_time_t(creationTime);

            // Truncate any precision after the whole number of seconds. TODO: [todo] We may want to change this to be as precise as
            // possible, but `boost::filesystem::creation_time` is not returning us the full precision value anyways, so unless
            // we want to address that, we can just keep it like this. Storing only the whole numbers look more user-friendly as well so maybe
            // we'll end up wanting it this way anyway. It depends on the use cases.
            std::chrono::time_point creationTimePointInDesiredDuration =
                std::chrono::time_point_cast<std::chrono::seconds>(creationTimePoint);

            // Convert the time point to a zoned time, relative to the local system's current UTC offset.
            return std::chrono::zoned_time{
                std::chrono::current_zone(),
                creationTimePointInDesiredDuration
            };
        }();

        // TODO: Avoid the unnecessary string allocation via a custom char buffer along with `std::format_to_n`.
        auto dateCreatedString = std::format(CPPUTILS_FORMAT_TIME_ISO_FULL_STRING_LITERAL, dateCreatedZonedTime);

        dcmiDateCreatedJsonString.SetString(dateCreatedString.data(), dateCreatedString.length(), metadataJsonDocument.GetAllocator());
    }

    // See: https://dublincore.org/specifications/dublin-core/dcmi-terms/terms/created/.
    dcmiDateJsonObject.AddMember("Created", std::move(dcmiDateCreatedJsonString), metadataJsonDocument.GetAllocator());

    dcmiJsonObject.AddMember("Date", std::move(dcmiDateJsonObject), metadataJsonDocument.GetAllocator());

    metadataJsonDocument.AddMember("DCMI", std::move(dcmiJsonObject), metadataJsonDocument.GetAllocator());
}
