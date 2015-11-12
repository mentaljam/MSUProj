#ifndef MSUPROJ_H
#define MSUPROJ_H


#include <msucore.h>


namespace msumr {

/**
 * @brief The class for reprojecting geo data
 * of the sensing equipment MSU-MR of the ERS satellite Meteor-M
 *
 * Uses GDAL (http://www.gdal.org/) to operate with rasters
 * and SGP4 C++ library (https://www.danrw.com/public/sgp4/) to calculate geogrids.
 */
class MSUPROJ_EXPORT MSUProj
{

public:

    /**
     * @brief A constructor
     */
    MSUProj();

    /**
     * @brief A destructor
     */
    ~MSUProj();

    /**
     * @brief The enumiration for getVersion() method
     */
    enum VERSION_TYPE
    {
        VERSION_STRING, ///< Return a version string
        VERSION_DATE,   ///< Return a version date
        VERSION_ARCH    ///< Return a version architecture
    };

    /**
     * @brief A method of getting version of MSUProj
     * @param type - a type of a returning value:
     * - 0 - Get version number
     * - 1 - Get build date
     * - 2 - Get build architecture
     * @return Version value
     */
    const char* getVersion(VERSION_TYPE type = VERSION_STRING) const;

    /**
     * @brief Amethod of setting a destination file name
     * @param file A destination file name
     */
    void setDst(std::string file);

    /**
     * @brief A method of setting a destination file format
     * @param format A destination file format
     *
     * Select one of GDAL raster formats (http://www.gdal.org/formats_list.html)
     * that supports creation. Default is 'GeoTiff'.
     */
    void setDstFormat(std::string format);

    /**
     * @brief A method of setting a source raster file
     * @param file A source file name
     * @return Exit code MSUMR::RETURN_CODE:
     * - success - Successful reading of a source file
     * - errSRC - Error of reading of a source file
     */
    const RETURN_CODE setSrc(std::string file);

    /**
     * @brief A  method of setting a GCP file
     * @param file A GCP file name
     * @return Exit code MSUMR::RETURN_CODE:
     * - success - Successful reading of a GCP file
     * - errGCP - Error of reading of a GCP file
     */
    const RETURN_CODE readGCP(std::string file);

    /**
     * @brief A method of getting an UTM zone string.
     *
     * @return A UTM zone string in format \<hemisphere\>\<zone number\>.
     * For example 'N38'.
     */
    const std::string getUTM() const;

    /**
     * @brief A method of projecting of a MSU-MR frame
     * @return Exit code MSUMR::RETURN_CODE:
     * - success - Successful projection
     * - errDST - Error of creation of a destination raster
     * - errSRC - A source image is not set
     * - errGCP - A source GCP grid is not set
     */
    const RETURN_CODE warp(const bool &useUTM = false, const bool &zerosAsND = false);

    /**
     * @brief A source image X size getter method
     * @return A number of pixels in a line of a source image
     */
    unsigned int getSrcXSize() const;

    /**
     * @brief A source image Y size getter method
     * @return A number of lines of a source image
     */
    unsigned int getSrcYSize() const;

    /**
     * @brief A GCP grid X size getter method
     * @return A number of points in a line of a GCP grid
     */
    unsigned int getGCPXSize() const;

    /**
     * @brief A GCP grid Y size getter method
     * @return A number of lines of a GCP grid
     */
    unsigned int getGCPYSize() const;

    /**
     * @brief A GCP grid X step getter method
     * @return A X step size of a GCP grid
     */
    unsigned int getGCPXStep() const;

    /**
     * @brief A GCP grid Y step getter method
     * @return A Y step size of a GCP grid
     */
    unsigned int getGCPYStep() const;

    /**
     * @brief Check if add logotype option is enabled
     * @return Option state
     */
    bool ifAddLogo() const;

    /**
     * @brief Set add logotype option state
     * @param enabled Option state
     */
    void setAddLogo(bool enabled);

private:

    enum BORDER_SIDE
    {
        BORDER_TOP,
        BORDER_BUTTON,
        BORDER_RIGHT,
        BORDER_LEFT
    };

    RETURN_CODE calculateBorder(BORDER_SIDE side, const GCP *gcps,
                                const double *geoTransform,
                                double *border, const unsigned int &size) const;

    SqNode *sqSurface(const GCP *gcps, unsigned int squareSize,
                      unsigned int *xSize, unsigned int *ySize) const;

    msumr::TriNode *triSurface(const GCP *gcps, unsigned int *xSize, unsigned int *ySize) const;

private:

    /**
     * @brief The enumeration for calculation of an output image size
     */
    enum COMPARE_COORDS
    {
        MIN_LON, ///< Minimal longitude
        MAX_LON, ///< Maximal longitude
        MIN_LAT, ///< Minimal latitude
        MAX_LAT  ///< Maximal latitude
    };

    GDALDataset* mSrcDS;     ///< A source image path

    GCP* mGCPs;              ///< A GCPs grid

    std::string mDstFile;    ///< A destination image path
    std::string mDstFormat;  ///< A destination image format code

    bool mAddLogo;           ///< Add logotypes of NTs OMZ and GDAL onto output raster.

    bool mHemisphere;        ///< A destination image hemisphere:
                             /// - 0 - Southern
                             /// - 1 - Northern
    unsigned int mZone;      ///< A destination image UTM zone number

    unsigned int mGCPXSize;  ///< A number of points in a line of a GCP grid
    unsigned int mGCPYSize;  ///< A number of lines of a GCP grid
    unsigned int mGCPXStep;  ///< A X step size of a GCP grid
    unsigned int mGCPYStep;  ///< A Y step size of a GCP grid
    unsigned int mGCPSize;   ///< A total number of points in a GCP grid
};

}

#endif // MSUPROJ_H
