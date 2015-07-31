#ifndef MSUCORE_H
#define MSUCORE_H


#include <gdal_priv.h>
#if _MSC_VER && !__INTEL_COMPILER
#   include <msuproj/msuproj_export.h>
#else
#   define MSUPROJ_EXPORT
#endif


namespace MSUMR {

static const unsigned short lineWidth = 1568;

/**
 * @brief Enumeration of exit codes
 */
enum retCode
{
    success, ///< Successful exit
    errSRC,  ///< Error of reading of source raster
    errDST,  ///< Error of creation of destination raster
    errGCP,  ///< Error of reading of GCP file
    errStat  ///< Error of reading stat file
};

/**
 * @brief Ground control point (GCP) structure
 */
struct gcp
{
    int x;      ///< The row of the source data matrix
    int y;      ///< The line of the source data matrix
    double lon; ///< Latitude
    double lat; ///< Longitude
};

/**
 * @ru
 * @brief Метод замены десятичного разделителя:
 * запятые (,) на точки (.)
 * @param str Исходная строка
 * @return Преобразованная строка
 */
const std::string comma2dot(std::string str);

}

#endif // MSUCORE_H

