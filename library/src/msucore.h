#ifndef MSUCORE_H
#define MSUCORE_H

#include <msuproj_version.h>
#include <msuproj_export.h>

#include <string>

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
 * @brief The decimal point replacement function
 *
 * Replaces all commas (,) of the source string to dots (.)
 * @param str - the source string
 * @return The output string
 */
const std::string comma2dot(std::string str);

}

#endif // MSUCORE_H

