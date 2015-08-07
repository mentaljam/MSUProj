#ifndef MSUPROJ_H
#define MSUPROJ_H


#include <msucore.h>


namespace MSUMR {

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
     * @brief A method of getting version of MSUProj
     * @param type - a type of a returning value:
     * - 0 - Get version number
     * - 1 - Get build date
     * - 2 - Get build architecture
     * @return Version value
     */
    const char* getVersion(const unsigned int &type = 0) const;

    /**
     * @brief Amethod of setting a destination file name
     * @param file A destination file name
     */
    void setDST(std::string file);

    /**
     * @brief A method of setting a destination file format
     * @param format A destination file format
     *
     * Select one of GDAL raster formats (http://www.gdal.org/formats_list.html)
     * that supports creation. Default is 'GeoTiff'.
     */
    void setDSTFormat(std::string format);

    /**
     * @brief A method of setting a source raster file
     * @param file A source file name
     * @return Exit code MSUMR::retCode:
     * - success - Successful reading of a source file
     * - errSRC - Error of reading of a source file
     */
    const retCode setSRC(std::string file);

    /**
     * @brief A  method of setting a GCP file
     * @param file A GCP file name
     * @return Exit code MSUMR::retCode:
     * - success - Successful reading of a GCP file
     * - errGCP - Error of reading of a GCP file
     */
    const retCode readGCP(std::string file);

    /**
     * @brief A method of setting a maximum size
     * of the pixel values interpolation block side
     *
     * When warping we try to calculate the average pixel value from
     * a block of near pixels. By default the maximal block size is 9x9.
     *
     * Set a less size to increase the warping speed.
     * Too small values will cause graininess and black areas.
     * Too big will blur image.
     * @param perim A block side size in pixels
     */
    void setPerimSize(const unsigned int &perim);

    /**
     * @brief A method of getting an UTM zone string.
     *
     * @return A UTM zone string in format \<hemisphere\>\<zone number\>.
     * For example 'N38'.
     */
    const std::string getUTM() const;

    /**
     * @brief A method of projecting of a MSU-MR frame
     * @return Exit code MSUMR::retCode:
     * - success - Successful projection
     * - errDST - Error of creation of a destination raster
     * - errSRC - A source image is not set
     * - errGCP - A source GCP grid is not set
     */
    const retCode warp(const bool &useUTM = false, const bool &zerosAsND = false);

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
    unsigned int getGcpXSize() const;

    /**
     * @brief A GCP grid Y size getter method
     * @return A number of lines of a GCP grid
     */
    unsigned int getGcpYSize() const;

    /**
     * @brief A GCP grid X step getter method
     * @return A X step size of a GCP grid
     */
    unsigned int getGcpXStep() const;

    /**
     * @brief A GCP grid Y step getter method
     * @return A Y step size of a GCP grid
     */
    unsigned int getGcpYStep() const;

private:

    /**
     * @brief The enumeration for calculation of an output image size
     */
    enum compareCoords
    {
        minLON, ///< Minimal longitude
        maxLON, ///< Maximal longitude
        minLAT, ///< Minimal latitude
        maxLAT  ///< Maximal latitude
    };

    GDALDataset* srcDS;     ///< A source image path

    gcp* gcps;              ///< A GCPs grid

    std::string dstFile;    ///< A destination image path
    std::string dstFormat;  ///< A destination image format code

    bool hemisphere;        ///< A destination image hemisphere:
                            /// - 0 - Southern
                            /// - 1 - Northern
    unsigned int zone;      ///< A destination image UTM zone number

    unsigned int perimSize; ///< A maximal interpolation block side size in pixels

    unsigned int srcXSize;  ///< A number of pixels in a line of a source image
    unsigned int srcYSize;  ///< A number of lines of a source image
    unsigned int srcSize;   ///< A total number of pixels in a source image

    unsigned int gcpXSize;  ///< A number of points in a line of a GCP grid
    unsigned int gcpYSize;  ///< A number of lines of a GCP grid
    unsigned int gcpXStep;  ///< A X step size of a GCP grid
    unsigned int gcpYStep;  ///< A Y step size of a GCP grid
    unsigned int gcpSize;   ///< A total number of points in a GCP grid

    /**
     * An analog of the GDAL GeoTransform affine transformation coefficients, where
     * - geoTransform[0] - top left X
     * - geoTransform[1] - X pixel resolution
     * - geoTransform[2] - X turning (0 for the North orientation)
     * - geoTransform[3] - top left Y
     * - geoTransform[4] - Y turning (0 for the North orientation)
     * - geoTransform[5] - Y pixel resolution (negative value)
     */
    double *geoTransform;

};

}

#endif // MSUPROJ_H
