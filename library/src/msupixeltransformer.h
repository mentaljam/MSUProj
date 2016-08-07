#ifndef PIXELTRANSFORMER_H
#define PIXELTRANSFORMER_H

#include <msutriangle.h>

#include <vector>

namespace msumr
{

class MSUPROJ_EXPORT PixelTransformer
{
public:
    PixelTransformer(const unsigned int &raster_x_size, const unsigned int &raster_y_size,
                     const unsigned int &gcpgrid_x_size, const unsigned int &gcpgrid_y_size,
                     GCP *gcps, double *geo_transform);
    PixelTransformer(const PixelTransformer &other);
    ~PixelTransformer();

    bool transform(const unsigned int &dst_x, const unsigned int &dst_y,
                   const double &lon, const double &lat,
                   double *src_x, double *src_y) const;

private:
    double *mNet;
    double *mRows;
    double *mCols;
    std::vector<Triangle> mTriangles;
    unsigned int mRowSize;
    unsigned int mColSize;
    unsigned int mRowsCount;
    unsigned int mColsCount;
};

}

#endif // PIXELTRANSFORMER_H
