#ifndef MSUPROJ_H
#define MSUPROJ_H

#include <msugcpgrid.h>

#include <gdal_priv.h>

namespace msumr {

/**
 * @brief The class for projecting images of the sensing equipment
 * MSU-MR of the Earth remote sensing satellite Meteor-M
 *
 * Uses GDAL (http://www.gdal.org/) to operate with rasters
 * and SGP4 C++ library (https://www.danrw.com/public/sgp4/) to calculate geogrids.
 */
class MSUPROJ_EXPORT MSUProj
{

public:

    /**
     * @brief The constructor
     */
    MSUProj();

    /**
     * @brief The copy constructor
     */
    MSUProj(const MSUProj &other);

    /**
     * @brief The destructor
     */
    ~MSUProj();

    /**
     * @brief The enumiration for the version() method
     */
    enum VERSION_TYPE
    {
        VERSION_STRING, ///< Return the version string
        VERSION_DATE,   ///< Return the version date
        VERSION_ARCH    ///< Return the version architecture
    };

    /**
     * @brief Returns the build information of MSUProj
     * @param type - the type of the returning value
     * @return the build information
     */
    const char* version(VERSION_TYPE type = VERSION_STRING) const;

    /**
     * @brief Sets the destination file name
     * @param file - the destination file name
     */
    void setDst(std::string file);

    /**
     * @brief Sets the destination file format
     * @param format - the destination file format
     *
     * Must be a code of one of the GDAL raster formats
     * (http://www.gdal.org/formats_list.html)
     * that supports creation. Default is 'GeoTiff'.
     */
    void setDstFormat(std::string format);

    /**
     * @brief Sets the source raster file
     * @param file - the source file name
     * @return The exit code:
     * - msumr::SUCCESS   - successful reading of the source file
     * - msumr::ERROR_SRC - the source file reading error
     */
    const RETURN_CODE setSrc(std::string file);

    /**
     * @brief Sets the GCP file
     * @param file - the GCP file name
     * @return The exit code:
     * - msumr::SUCCESS - successful reading of the GCP file
     * - msumr::ERROR_GCP - the GCP file reading error
     */
    const RETURN_CODE readGCP(std::string file);

    /**
     * @brief Project the MSU-MR image
     * @return The exit code:
     * - msumr::SUCCESS - successful projection
     * - msumr::ERROR_DST - the destination raster creation error
     * - msumr::ERROR_SRC - the source image is not set
     * - msumr::ERROR_GCP - the source GCP grid is not set
     */
    const RETURN_CODE warp(const bool &useUTM = false, const bool &zerosAsND = false);

    /**
     * @brief Returns the source image X size
     * @return The number of pixels in the a of the source image
     */
    unsigned int srcXSize() const;

    /**
     * @brief Returns the source image Y size
     * @return The number of lines of the source image
     */
    unsigned int srcYSize() const;

    /**
     * @brief Returns the 'Add logotype' option state
     * @return The option state
     */
    bool ifAddLogo() const;

    /**
     * @brief Sets the 'Add logotype' option state
     * @param enabled - the option state
     */
    void setAddLogo(bool enabled);

    unsigned int *progressMaxPtr();

    unsigned int *progressValPtr();

    GCPGrid *gcpGrid() const;

private:

    /**
     * @brief The enumeration for the calculateBorder() method
     */
    enum BORDER_SIDE
    {
        BORDER_TOP,    ///< 0 - the image top border
        BORDER_BOTTOM, ///< 1 - the image button border
        BORDER_RIGHT,  ///< 2 - the image right border
        BORDER_LEFT    ///< 3 - the image left border
    };

    /**
     * @brief Calculates real image borders in raster extent
     *
     * The border is an array of doubles for every raster pixel of provided lenght.
     * For example for the raster with the line width = 1000 the top border
     * will contain values of maximum latitudes for every pixel (1000 in summary).
     *
     * @param side - the type of the border
     * @param gcps - the array of GCPs for borders calculation
     * @param geoTransform - the image affine transformation coefficients
     * (http://www.gdal.org/gdal_datamodel.html, http://www.gdal.org/gdal_tutorial.html)
     * @param border - the border array (must be an initialized pointer)
     * @param size - the border lenght
     * @return The exit code:
     * - msumr::SUCCESS - the border was successfully calculated
     * - msumr::ERROR_ARG - the bad argument error
     * (gcps == 0 || border == 0 || geoTransform == 0)
     */
    RETURN_CODE calculateBorder(BORDER_SIDE side, const GCP *gcps,
                                const double *geoTransform,
                                double *border, const unsigned int &size) const;

private:

    bool mAddLogo;           ///< Add logotypes of NTs OMZ and GDAL onto output raster.

    GDALDataset* mSrcDS;     ///< A source image path

    GCPGrid *mGcpGrid;

    std::string mDstFile;    ///< A destination image path

    std::string mDstFormat;  ///< A destination image format code

    unsigned int mProgressMax;
    unsigned int mProgressVal;
};

}

#endif // MSUPROJ_H
