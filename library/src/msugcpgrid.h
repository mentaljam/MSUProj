#ifndef MSUGCPGRID_H
#define MSUGCPGRID_H

#include <msucore.h>
#include <msupixeltransformer.h>
#include <ogrsf_frmts.h>

namespace msumr
{

class MSUPROJ_EXPORT GCPGrid
{
public:

    GCPGrid();
    GCPGrid(const GCPGrid &other);
    ~GCPGrid();

    bool isUtmNorthern() const;

    unsigned short utmZone() const;

    /**
     * @brief Returns the UTM zone string.
     *
     * @return The UTM zone string in format
     * @code
     * <hemisphere><zone number>
     * @endcode
     * For example '38N'.
     */
    std::string utmZoneName() const;

    GCP *gcps() const;

    OGRSpatialReference srs() const;

    size_t size() const;

    double *geoTransform() const;

    size_t extentXSize() const;

    size_t extentYSize() const;

    bool fromFile(const std::string &path);

    bool toUtm();

    GCP *operator[] (const size_t &index) const;

    /**
     * @brief Returns number of points in a line of the GCP grid
     * @return Number of points in a line of the GCP grid
     */
    unsigned int xSize() const;

    /**
     * @brief Returns number of lines of the GCP grid
     * @return Number of lines of the GCP grid
     */
    unsigned int ySize() const;

    PixelTransformer *pixelTransformer() const;

private:

    /**
     * @brief The enumeration for calculation of the output image size
     */
    enum COMPARE_COORDS
    {
        MIN_LON, ///< Minimal longitude
        MAX_LON, ///< Maximal longitude
        MIN_LAT, ///< Minimal latitude
        MAX_LAT  ///< Maximal latitude
    };

    bool mUtmNorthern;        ///< The hemisphere of the output image:
                              /// - true - Southern
                              /// - false - Northern
    unsigned short mUtmZone;  ///< The UTM zone number of the output image
    GCP *mGcps;               ///< The grid of GCPs
    OGRSpatialReference mSrs; ///< The output image spatial reference
    unsigned int mXSize;      ///< Number of points in a line of the GCP grid
    unsigned int mYSize;      ///< Number of lines of the GCP grid
    size_t mSize;             ///< Total number of points in the GCP grid
    size_t mExtentXSize;      ///< The output image width
    size_t mExtentYSize;      ///< The output image height
    double mGeoTransform[6];  ///< The output image transformation affine coefficients

    void pUpdateVars(bool utm = false);
};

}

#endif // MSUGCPGRID_H
