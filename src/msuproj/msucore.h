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
    SUCCESS,   ///< Successful exit
    ERROR_ARG,
    ERROR_SRC, ///< Error of reading of a source raster
    ERROR_DST, ///< Error of creation of a destination raster
    ERROR_GCP, ///< Error of reading of a GCP file
    ERROR_STAT ///< Error of reading a stat file
};

struct SqNode
{
    unsigned int GCP0;
    double lon; ///< A latitude
    double lat; ///< A longitude
};

/**
 * @brief The Ground control point (GCP) structure
 */
struct GCP
{
    int x;      ///< A column of a source data matrix
    int y;      ///< A line of a source data matrix
    double lon; ///< A latitude
    double lat; ///< A longitude
};

struct TriNode
{
    int row0;
    int col0;
    double aRow;
    double bRow;
    double aCol;
    double bCol;
    double c;
    double lat0;
    double lon0;
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

