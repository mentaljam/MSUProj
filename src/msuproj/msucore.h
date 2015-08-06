#ifndef MSUCORE_H
#define MSUCORE_H


#include <gdal_priv.h>
#if _MSC_VER && !__INTEL_COMPILER
#   include <msuproj/msuproj_export.h>
#else
#   define MSUPROJ_EXPORT
#endif


namespace MSUMR {

static const unsigned short lineWidth = 1568; ///< The MSU-MR line width

/**
 * @brief The enumeration of exit codes
 */
enum retCode
{
    success, ///< Successful exit
    errSRC,  ///< Error of reading of a source raster
    errDST,  ///< Error of creation of a destination raster
    errGCP,  ///< Error of reading of a GCP file
    errStat  ///< Error of reading a stat file
};

/**
 * @brief The Ground control point (GCP) structure
 */
struct gcp
{
    int x;      ///< A row of a source data matrix
    int y;      ///< A line of a source data matrix
    double lon; ///< A latitude
    double lat; ///< A longitude
};

/**
 * @brief The decimal point replacement function
 *
 * Replaces all commas (,) of a source string to dots (.)
 * @param str A source string
 * @return An output string
 */
const std::string comma2dot(std::string str);

}

#endif // MSUCORE_H

