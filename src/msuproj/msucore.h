#ifndef MSUCORE_H
#define MSUCORE_H


#include <gdal_priv.h>
#if _MSC_VER && !__INTEL_COMPILER
#   include <msuproj/msuproj_export.h>
#else
#   define MSUPROJ_EXPORT
#endif


namespace msumr {

static const unsigned short lineWidth = 1568; ///< The MSU-MR line width

/**
 * @brief The enumeration of exit codes
 */
enum RETURN_CODE
{
    SUCCESS,   ///< 0 - successful exit
    ERROR_ARG, ///< 1 - the bad argument error
    ERROR_SRC, ///< 2 - the source raster reading error
    ERROR_DST, ///< 3 - the destination raster creation error
    ERROR_GCP, ///< 4 - the GCP file reading error
    ERROR_STAT ///< 5 - the stat file reading error
};

/**
 * @brief Stores coordinates of the center
 * of the node (quadrangle) of GCPs of given size
 */
struct qdrNode
{
    unsigned int firstRow; ///< The row of the first GCP in the node
    unsigned int firstCol; ///< The column of the first GCP in the node
    double lon;            ///< The node center point latitude
    double lat;            ///< The node center point longitude
};

/**
 * @brief The Ground control point (GCP) structure
 */
struct GCP
{
    int x;      ///< The point column in the source data matrix
    int y;      ///< The point line in the source data matrix
    double lon; ///< The point latitude
    double lat; ///< The point longitude
};

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
struct TriNode
{
    int row0;    ///< The source image row of the first point
    int col0;    ///< The source image column of the first point
    double aRow; ///< The A coefficient for a row calculation
    double bRow; ///< The B coefficient for a row calculation
    double aCol; ///< The A coefficient for a column calculation
    double bCol; ///< The B coefficient for a column calculation
    double c;    ///< The C coefficient
    double lat0; ///< The transformed image row of the first point
    double lon0; ///< The transformed image column of the first point
};

/**
 * @brief The decimal point replacement function
 *
 * Replaces all commas (,) of the source string to dots (.)
 * @param str - the source string
 * @return The output string
 */
const std::string comma2dot(std::string str);

}

#endif // MSUCORE_H

