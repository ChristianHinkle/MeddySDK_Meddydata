// Copyright (c) 2023-2025 Christian Hinkle, Brian Hinkle.

#ifndef MEDDYSDK_MEDDYDATA_INCLUDE_EXPORT_H
#define MEDDYSDK_MEDDYDATA_INCLUDE_EXPORT_H

// Since interface libraries cannot generate header files during their cmake configuration, we author our
// own "export header" file here, which defines the export macros as a noop.

#define MEDDYSDK_MEDDYDATA_EXPORT
#define MEDDYSDK_MEDDYDATA_NO_EXPORT

#endif // #ifndef MEDDYSDK_MEDDYDATA_INCLUDE_EXPORT_H
