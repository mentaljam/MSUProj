#include "msupixeltransformer.h"

#include <cstring>

msumr::PixelTransformer::PixelTransformer(const unsigned int &raster_x_size, const unsigned int &raster_y_size,
                                          const unsigned int &gcpgrid_x_size, const unsigned int &gcpgrid_y_size,
                                          GCP *gcps, double *geo_transform) :
    mNet(new double[raster_x_size * gcpgrid_y_size + raster_y_size * gcpgrid_x_size]),
    mRows(&mNet[0]),
    mCols(&mNet[raster_x_size * gcpgrid_y_size]),
    mRowSize(raster_x_size),
    mColSize(raster_y_size),
    mRowsCount(gcpgrid_y_size),
    mColsCount(gcpgrid_x_size)
{
    // Direct or reverse image
    bool direct = gcps[0].lon < gcps[gcpgrid_x_size - 1].lon;

    // GCP grid total size
    size_t gcpSize  = gcpgrid_x_size * gcpgrid_y_size;

    // Triangles initialization
    unsigned int yLimit = gcpgrid_y_size - 1;
    unsigned int xLimit = gcpgrid_x_size - 1;
    if (direct)
    {
        for (unsigned int row = 0; row < yLimit; ++row)
        {
            size_t a = row * gcpgrid_x_size;
            size_t b = a + 1;
            size_t c = a + gcpgrid_x_size;
            for (unsigned int col = 0; col < xLimit; ++col, ++a, ++b, ++c)
                mTriangles.push_back(Triangle(gcps[a], gcps[b], gcps[c]));
        }
    }
    else
    {
        for (unsigned int row = 0; row < yLimit; ++row)
        {
            size_t a = gcpSize - row * gcpgrid_x_size - 1;
            size_t b = a - 1;
            size_t c = a - gcpgrid_x_size;
            for (unsigned int col = 0; col < xLimit; ++col, --a, --b, --c)
                mTriangles.push_back(Triangle(gcps[a], gcps[b], gcps[c]));
        }
    }

    // Net calculation
    int gcpInc;
    int gcpLimitInc;
    size_t gcpStart;
    size_t gcpEnd;

    // 1) rows
    if (direct)
    {
        gcpStart    = 0;
        gcpEnd      = gcpgrid_x_size - 1;
        gcpInc      = 1;
        gcpLimitInc = gcpgrid_x_size;
    }
    else
    {
        gcpStart    = gcpSize - 1;
        gcpEnd      = gcpSize - gcpgrid_x_size;
        gcpInc      = -1;
        gcpLimitInc = -gcpgrid_x_size;
    }
    for (unsigned int row = 0, i = 0;
         row < mRowsCount;
         ++row, gcpStart += gcpLimitInc, gcpEnd += gcpLimitInc)
    {
        size_t gcpInd = gcpStart + gcpInc;
        double lon    = geo_transform[0];
        double a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                   (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
        double b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
        for (unsigned int x = 0; x < mRowSize; ++x, lon += geo_transform[1], ++i)
        {
            if (lon > gcps[gcpInd].lon && gcpInd + gcpInc != gcpEnd)
            {
                gcpInd += gcpInc;
                a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                    (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
                b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
            }
            mRows[i] = a * lon + b;
        }
    }

    // 2) columns
    if (direct)
    {
        gcpStart    = 0;
        gcpEnd      = gcpSize - gcpgrid_x_size;
        gcpInc      = gcpgrid_x_size;
        gcpLimitInc = 1;
    }
    else
    {
        gcpStart    = gcpSize - 1;
        gcpEnd      = gcpgrid_x_size - 1;
        gcpInc      = -gcpgrid_x_size;
        gcpLimitInc = -1;
    }
    for (unsigned int col = 0, i = 0;
         col < mColsCount;
         ++col, gcpStart += gcpLimitInc, gcpEnd += gcpLimitInc)
    {
        size_t gcpInd = gcpStart + gcpInc;
        double lat    = geo_transform[3];
        double a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                   (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
        double b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
        for (unsigned int y = 0; y < mColSize; ++y, lat += geo_transform[5], ++i)
        {
            if (lat < gcps[gcpInd].lat && gcpInd + gcpInc != gcpEnd)
            {
                gcpInd += gcpInc;
                a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                    (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
                b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
            }
            mCols[i] = (lat - b) / a;
        }
    }
    --mRowsCount;
    --mColsCount;
}

msumr::PixelTransformer::PixelTransformer(const msumr::PixelTransformer &other) :
    mNet(new double[other.mRowSize * other.mRowsCount + other.mColSize * other.mColsCount]),
    mRows(&mNet[0]),
    mCols(&mNet[other.mRowSize * other.mRowsCount]),
    mRowSize(other.mRowSize),
    mColSize(other.mColSize),
    mRowsCount(other.mRowsCount),
    mColsCount(other.mColsCount)
{
    memcpy(mNet, other.mNet, other.mRowSize * other.mRowsCount +
           other.mColSize * other.mColsCount * sizeof(double));
}

msumr::PixelTransformer::~PixelTransformer()
{
    delete[] mNet;
}

bool msumr::PixelTransformer::transform(const unsigned int &dst_x, const unsigned int &dst_y,
                                        const double &lon, const double &lat,
                                        double *src_x, double *src_y) const
{
    // Skip pixels that are not in image borders
    if (lat > mRows[dst_x] || lat < mRows[mRowSize * mRowsCount + dst_x] ||
        lon < mCols[dst_y] || lon > mCols[mColSize * mColsCount + dst_y])
        return false;

    unsigned int col;
    unsigned int row;

    // Find nearest left indexes of the net columns and rows
    unsigned int left  = 0;
    unsigned int right = mColsCount;
    while (left < right - 1)
    {
        col = (left + right) / 2;
        if (lon < mCols[col * mColSize + dst_y])
            right = col;
        else
            left  = col;
    }
    col = (left + right) / 2;
    left  = 0;
    right = mRowsCount;
    while (left < right - 1)
    {
        row = (left + right) / 2;
        if (lat > mRows[row * mRowSize + dst_x])
            right = row;
        else
            left  = row;
    }
    row = (left + right) / 2;

    // Calculate pixel coordinates in the source image
    mTriangles[row * mColsCount + col].sourceCoordinates(lon, lat, src_x, src_y);

    return true;
}
