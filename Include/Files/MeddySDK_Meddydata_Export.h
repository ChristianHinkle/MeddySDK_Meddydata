// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#ifndef MEDDYSDK_MEDDYDATA_EXPORT_H
#define MEDDYSDK_MEDDYDATA_EXPORT_H

// Include the appropriate "export header" file depending on which type of this library is being compiled.

#if __has_include(<meddysdk_meddydata_shared_export.h>)
#   include <meddysdk_meddydata_shared_export.h>
#elif __has_include(<meddysdk_meddydata_module_export.h>)
#   include <meddysdk_meddydata_module_export.h>
#elif __has_include(<meddysdk_meddydata_object_export.h>)
#   include <meddysdk_meddydata_object_export.h>
#elif __has_include(<meddysdk_meddydata_static_export.h>)
#   include <meddysdk_meddydata_static_export.h>
#else
    // Since no generated header files exist, it must mean we are an interface library (being used as a header-only library).
#   include <meddysdk_meddydata_include_export.h>
#endif

// Undef the generated macros that we've decided not to use.

#ifdef MEDDYSDK_MEDDYDATA_DEPRECATED
#   undef MEDDYSDK_MEDDYDATA_DEPRECATED
#endif

#ifdef MEDDYSDK_MEDDYDATA_DEPRECATED_EXPORT
#   undef MEDDYSDK_MEDDYDATA_DEPRECATED_EXPORT
#endif

#ifdef MEDDYSDK_MEDDYDATA_DEPRECATED_NO_EXPORT
#   undef MEDDYSDK_MEDDYDATA_DEPRECATED_NO_EXPORT
#endif

#ifdef MEDDYSDK_MEDDYDATA_NO_DEPRECATED
#   undef MEDDYSDK_MEDDYDATA_NO_DEPRECATED
#endif

#endif // #ifndef MEDDYSDK_MEDDYDATA_EXPORT_H
