// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK/Meddydata/Utils.h>

#include <rapidjson/document.h>
#include <cassert>

template <class Encoding, class Allocator, class StackAllocator>
void MeddySDK::AppendNewMetadataJson(const MeddySDK::Meddydata& meddydata, rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>& metadataJsonDocument)
{
    assert(metadataJsonDocument.IsObject()); // The root must be an object.

    // Populate the metadata.json with any essential information for Meddy here. Or maybe we should make a separate file for this. Maybe there
    // should be a separate file named "manifest.json" or something like that. And then "metadata.json" could be 100% owned by MeddySDK_DAM library.
}
