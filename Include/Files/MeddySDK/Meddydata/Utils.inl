// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#pragma once

#include <MeddySDK/Meddydata/Utils.h>

#include <rapidjson/document.h>

// See TODO below.

//#include <MeddySDK/DAM/Utils.h>

template <class Encoding, class Allocator, class StackAllocator>
void MeddySDK::AppendNewMetadataJson(const MeddySDK::Meddydata& meddydata, rapidjson::GenericDocument<Encoding, Allocator, StackAllocator>& metadataJsonDocument)
{
    assert(metadataJsonDocument.IsObject()); // The root must be an object.

    // TODO: [todo][deps] Solidify the design more to connect this together. We can't have Meddydata library depend on DAM because that
    // would be circular, and we shouldn't anyway because that would be a bad design. Figure out how we want this metadata.json file to work. One
    // idea: MeddySDK_Meddydata just creates an a purely empty file of configurable name, the user of this SDK chooses the file name and
    // tells MeddySDK_DAM to populate it.

    //AppendDcmiMetadataJson(meddydata, metadataJsonDocument);
}
