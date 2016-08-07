#include "msutriangle.h"

msumr::Triangle::Triangle(GCP &vertex1, GCP &vertex2, GCP &vertex3) :
    mVertex0(&vertex1)
{
    double x10, x20, y10, y20, z10R, z20R, z10L, z20L;
    x10  = vertex2.lon - vertex1.lon;
    x20  = vertex3.lon - vertex1.lon;
    y10  = vertex2.lat - vertex1.lat;
    y20  = vertex3.lat - vertex1.lat;
    z10R = vertex2.x - vertex1.x;
    z20R = vertex3.x - vertex1.x;
    z10L = vertex2.y - vertex1.y;
    z20L = vertex3.y - vertex1.y;
    mACol = y10 * z20R - z10R * y20;
    mBCol = x10 * z20R - x20 * z10R;
    mARow = y10 * z20L - z10L * y20;
    mBRow = x10 * z20L - x20 * z10L;
    mC  = x10 * y20 - x20 * y10;
}

void msumr::Triangle::sourceCoordinates(const double &dst_x, const double &dst_y,
                                              double *src_x, double *src_y) const
{
    double deltaX = dst_x - mVertex0->lon;
    double deltaY = dst_y - mVertex0->lat;
    *src_x = mVertex0->x - (deltaX * mACol - deltaY * mBCol) / mC;
    *src_y = mVertex0->y - (deltaX * mARow - deltaY * mBRow) / mC;
}

