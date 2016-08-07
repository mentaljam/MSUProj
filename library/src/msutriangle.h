#ifndef MSUTRIANGLE_H
#define MSUTRIANGLE_H

#include <msucore.h>

namespace msumr
{
/**
 * @brief Stores coefficients for calculating
 * pixel row and column in source image
 * based on its coordinates in transformed image
 *
 * Calculation is based on a plane:
 *
 * @code
 *
 * GCP1  GCP2
 *  *--*
 *  | /
 *  *
 * GCP3
 *
 *         | x1 y1 z1 |
 * plane = | x2 y2 z2 |
 *         | x3 y3 z3 |
 *
 * Ax + By + C = 0
 *
 * @endcode
 *
 * where
 *
 * @code
 * x = GCP.lon
 * y = GCP.lat
 * z = GCP.x or GCP.y
 * A, B, X - coefficients
 * @endcode
 */
class MSUPROJ_EXPORT Triangle
{
public:
    Triangle(GCP &vertex1, GCP &vertex2, GCP &vertex3);
    void sourceCoordinates(const double &dst_x, const double &dst_y,
                           double *src_x, double *src_y) const;

private:
    GCP *mVertex0;
    double mARow; ///< The A coefficient for a row calculation
    double mBRow; ///< The B coefficient for a row calculation
    double mACol; ///< The A coefficient for a column calculation
    double mBCol; ///< The B coefficient for a column calculation
    double mC;    ///< The C coefficient
};

}

#endif // MSUTRIANGLE_H
