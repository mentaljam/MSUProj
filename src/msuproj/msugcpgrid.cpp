#include <msugcpgrid.h>
#include <fstream>
#include <ogrsf_frmts.h>
#include <set>
#include <cfloat>

msumr::GCPGrid::GCPGrid() :
    mUtmNorthern(true),
    mUtmZone(0),
    mGcps(0),
    mSrs(0),
    mXSize(0),
    mYSize(0),
    mSize(0),
    mExtentXSize(0),
    mExtentYSize(0)
  // A workaround for Visual Studio 2013
#if _MSC_VER != 1800
    , mGeoTransform{0.0}
{
#else
{
    memset(mGeoTransform, 0, 6 * sizeof(double));
#endif
}

msumr::GCPGrid::GCPGrid(const msumr::GCPGrid &other) :
    mUtmNorthern(other.mUtmNorthern),
    mUtmZone(other.mUtmZone),
    mGcps(new GCP[other.mSize]),
    mSrs(other.mSrs),
    mXSize(other.mXSize),
    mYSize(other.mYSize),
    mSize(other.mSize),
    mExtentXSize(other.mExtentXSize),
    mExtentYSize(other.mExtentYSize)
{
    memcpy(mGcps, other.mGcps, sizeof(GCP) * mSize);
    memcpy(mGeoTransform, other.mGeoTransform, sizeof(double) * 6);
}

msumr::GCPGrid::~GCPGrid()
{
    if (mGcps)
        delete[] mGcps;
}

bool msumr::GCPGrid::isUtmNorthern() const
{
    return mUtmNorthern;
}

unsigned short msumr::GCPGrid::utmZone() const
{
    return mUtmZone;
}

std::string msumr::GCPGrid::utmZoneName() const
{
    if (!mGcps)
        return "unknown";
    auto name = std::to_string(mUtmZone);
    if (mUtmNorthern)
        return name + "N";
    else
        return name + "S";
}

msumr::GCP *msumr::GCPGrid::gcps() const
{
    return mGcps;
}

OGRSpatialReference msumr::GCPGrid::srs() const
{
    return mSrs;
}

size_t msumr::GCPGrid::size() const
{
    return mSize;
}

double *msumr::GCPGrid::geoTransform() const
{
    return (double*)mGeoTransform;
}

size_t msumr::GCPGrid::extentXSize() const
{
    return mExtentXSize;
}

size_t msumr::GCPGrid::extentYSize() const
{
    return mExtentYSize;
}

bool msumr::GCPGrid::fromFile(const std::string &path)
{
    std::ifstream gcpFile(path);
    if (gcpFile.fail())
        return false;

    if (mGcps)
    {
        delete[] mGcps;
        mXSize = 0;
        mYSize = 0;
        mSize = 0;
    }

    std::string line;

    while (getline(gcpFile, line))
        ++mSize;
    if (mSize == 0)
        return false;
    gcpFile.clear();
    gcpFile.seekg(0);

    mGcps = new GCP[mSize];
    size_t gcpInd = 0;
    size_t pos;
    while (getline(gcpFile, line))
    {
        mGcps[gcpInd].x = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        mGcps[gcpInd].y = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        line = comma2dot(line);

        mGcps[gcpInd].lat = std::stod(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        mGcps[gcpInd].lon = std::stod(line, &pos);

        ++gcpInd;
    }

    unsigned int firstCol = mGcps[0].x;
    while (firstCol != mGcps[++mXSize].x);
    if (mSize % mXSize > 0)
        return false;
    mYSize = mSize / mXSize;

    mSrs.importFromEPSG(4326);
    this->pUpdateVars();

    return true;
}

bool msumr::GCPGrid::toUtm()
{
    auto srcSrs = mSrs;
    if (!mGcps || srcSrs.GetEPSGGeogCS() != 4326)
        return false;

    mSrs.SetUTM(mUtmZone, mUtmNorthern);
    auto dstSrs = mSrs;
    auto *transFunc = OGRCreateCoordinateTransformation(&srcSrs, &dstSrs);
    for (size_t i = 0; i < mSize; ++i)
        transFunc->Transform(1, &mGcps[i].lon, &mGcps[i].lat);
    this->pUpdateVars(true);

    return true;
}

msumr::GCP *msumr::GCPGrid::operator[] (const size_t &index) const
{
    return &mGcps[index];
}

unsigned int msumr::GCPGrid::xSize() const
{
    return mXSize;
}

unsigned int msumr::GCPGrid::ySize() const
{
    return mYSize;
}

msumr::PixelTransformer *msumr::GCPGrid::pixelTransformer() const
{
    return new PixelTransformer(mExtentXSize, mExtentYSize,
                                mXSize, mYSize,
                                mGcps, (double*)&mGeoTransform[0]);
}

void msumr::GCPGrid::pUpdateVars(bool utm)
{
    if (mGcps)
    {
        double lon = 0.0;
        double lat = 0.0;
        double coords[4];
        coords[MIN_LON] = mGcps[0].lon;
        coords[MAX_LON] = mGcps[0].lon;
        coords[MIN_LAT] = mGcps[0].lat;
        coords[MAX_LAT] = mGcps[0].lat;
        for (size_t i = 1; i < mSize; ++i)
        {
            lon += mGcps[i].lon;
            lat += mGcps[i].lat;
            if (coords[MIN_LON] > mGcps[i].lon)
                coords[MIN_LON] = mGcps[i].lon;
            if (coords[MAX_LON] < mGcps[i].lon)
                coords[MAX_LON] = mGcps[i].lon;
            if (coords[MIN_LAT] > mGcps[i].lat)
                coords[MIN_LAT] = mGcps[i].lat;
            if (coords[MAX_LAT] < mGcps[i].lat)
                coords[MAX_LAT] = mGcps[i].lat;
        }
        lon /= mSize;
        lat /= mSize;
        mGeoTransform[0] = coords[MIN_LON];
        mGeoTransform[3] = coords[MAX_LAT];
        if (!utm)
        {
            mGeoTransform[1] = 0.01;
            mGeoTransform[5] = -0.01;
            mUtmZone     = (int)(((int)(lon + 0.5) + 180) / 6) + 1;
            mUtmNorthern = (int)(lat + 0.5) > 0;
        }
        else
        {
            mGeoTransform[1] = 1000;
            mGeoTransform[5] = -1000;
        }
        mExtentXSize = (size_t)((coords[MAX_LON] - coords[MIN_LON]) / mGeoTransform[1] + 0.5);
        mExtentYSize = (size_t)((coords[MAX_LAT] - coords[MIN_LAT]) / mGeoTransform[1] + 0.5);
    }
}
