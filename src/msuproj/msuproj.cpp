#include <msuproj.h>
#include <logoimage.hpp>
#include <msugcpgrid.h>
#include <ogrsf_frmts.h>
#include <fstream>
#include <cfloat>
#include <vector>

msumr::MSUProj::MSUProj() :
    mAddLogo(true),
    mSrcDS(0),
    mGcpGrid(new GCPGrid),
    mDstFile(),
    mDstFormat("GTiff"),
    mProgressMax(0),
    mProgressVal(0)
{
    // Need this for reading GCP files because Qt sets locale to system
    // (error with decimal delimiter in stod function)
    setlocale(LC_NUMERIC, "C");
}

msumr::MSUProj::MSUProj(const msumr::MSUProj &other) :
    mAddLogo(other.mAddLogo),
    mSrcDS(other.mSrcDS),
    mGcpGrid(new GCPGrid(*other.mGcpGrid)),
    mDstFile(other.mDstFile),
    mDstFormat(other.mDstFormat),
    mProgressMax(0),
    mProgressVal(0)
{

}

msumr::MSUProj::~MSUProj()
{
    delete mGcpGrid;
}

const char *msumr::MSUProj::version(VERSION_TYPE type) const
{
    switch (type)
    {
    case VERSION_STRING:
        return VERSION_MSUPROJ_STRING;
        break;
    case VERSION_DATE:
        return VERSION_MSUPROJ_DATE;
        break;
    case VERSION_ARCH:
        return VERSION_MSUPROJ_ARCH;
        break;
    default:
        return "";
        break;
    }
}

void msumr::MSUProj::setDst(std::string file)
{
    mDstFile = file;
}

void msumr::MSUProj::setDstFormat(std::string format)
{
    mDstFormat = format;
}

const msumr::RETURN_CODE msumr::MSUProj::setSrc(std::string file)
{
    if (mSrcDS)
        GDALClose(mSrcDS);
    mSrcDS = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
    if (!mSrcDS)
        return ERROR_SRC;

    return SUCCESS;
}

const msumr::RETURN_CODE msumr::MSUProj::readGCP(std::string file)
{
    if (!mSrcDS)
        return ERROR_SRC;
    if (!mGcpGrid->fromFile(file))
        return ERROR_GCP;
    return SUCCESS;
}

const msumr::RETURN_CODE msumr::MSUProj::warp(const bool &useUtm, const bool &zerosAsND)
{
    if (!mSrcDS)
        return ERROR_SRC;

    if (mGcpGrid->size() == 0)
        return ERROR_GCP;

    if (mDstFile.empty())
        return ERROR_DST;

    unsigned short bands = mSrcDS->GetRasterCount();

    GCPGrid gcps = *mGcpGrid;
    if (useUtm)
        gcps.toUtm();
    auto dstXSize = gcps.extentXSize();
    auto dstYSize = gcps.extentYSize();
    auto dstSize  = dstXSize * dstYSize;
    auto geoTransform = gcps.geoTransform();

    unsigned short band;
    GDALDriver  *dstDriver = GetGDALDriverManager()->GetDriverByName(mDstFormat.c_str());
    GDALDataset *dstDS;
    if (mDstFormat == "GTiff")
    {
        char **dstOptions  = 0;
        char **dstMetadata = 0;
        dstOptions = CSLSetNameValue(dstOptions, "PHOTOMETRIC", "RGB");
        dstOptions = CSLSetNameValue(dstOptions, "COMPRESS", "JPEG");
        dstOptions = CSLSetNameValue(dstOptions, "JPEG_QUALITY", "100");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_IMAGEDESCRIPTION", "Meteor-M MSU-MR georeferenced image");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_SOFTWARE", "MSUProj v" VERSION_MSUPROJ_STRING);
        dstDS = dstDriver->Create(mDstFile.c_str(), dstXSize, dstYSize, bands, GDT_Byte, dstOptions);
        dstDS->SetMetadata(dstMetadata);
    }
    else
        dstDS = dstDriver->Create(mDstFile.c_str(), dstXSize, dstYSize, bands, GDT_Byte, 0);

    // Error creating destination raster
    if (dstDS == 0)
        return ERROR_DST;

    // Destination raster projection and transformation
    char *srsWKT;
    gcps.srs().exportToWkt(&srsWKT);
    dstDS->SetProjection(srsWKT);
    dstDS->SetGeoTransform(geoTransform);

    // Set zero as ND value
    if (zerosAsND)
        for (band = 1; band <= bands; ++band)
            dstDS->GetRasterBand(band)->SetNoDataValue(0);

    // Source raster size
    unsigned int srcXSize = mSrcDS->GetRasterXSize(),
                 srcYSize = mSrcDS->GetRasterYSize();
    unsigned int srcSize  = srcXSize * srcYSize;
    mProgressMax = srcYSize;

    // Pixel arrays
    unsigned char *pxlData = new unsigned char[(srcSize + dstSize) * bands];
    std::vector<unsigned char*> srcData(bands);
    std::vector<unsigned char*> dstData(bands);
    for (band = 0; band < bands; ++band)
    {
        srcData[band] = &pxlData[srcSize * band];
        dstData[band] = &pxlData[srcSize * bands + dstSize * band];
        // Read source data
        mSrcDS->GetRasterBand(band + 1)->RasterIO(GF_Read, 0, 0, srcXSize, srcYSize,
                                                  srcData[band],
                                                  srcXSize, srcYSize, GDT_Byte, 0, 0);
    }

    auto transformer = gcps.pixelTransformer();

    // Destination raster lines loop
    #pragma omp parallel for private(band)
    for (
// A workaround for MSVC that does not support OpenMP 3.0
#ifdef _MSC_VER
         int pRow = 0;
#else
         unsigned int pRow = 0;
#endif
         pRow < dstYSize; ++pRow)
    {
        ++mProgressVal;
        unsigned int dstInd = dstXSize * pRow;
        double lat = geoTransform[3] + geoTransform[5] * pRow;
        double lon = geoTransform[0];
        // Destination raster columns loop
        for (unsigned int pCol = 0; pCol < dstXSize; ++pCol, ++dstInd, lon += geoTransform[1])
        {
            double pRCol, pRRow;
            if (transformer->transform(pCol, pRow, lon, lat, &pRCol, &pRRow))
            {
                // Calculate pixel value (bilinear interpolation)
                int srcRow = (int)pRRow;
                int srcCol = (int)pRCol;
                if (srcRow < srcYSize - 2 && srcRow > 0 && srcCol < srcXSize - 2 && srcCol > 0)
                {
                    unsigned int srcInd = srcRow * srcXSize + srcCol;
                    pRRow -= srcRow;
                    pRCol -= srcCol;
                    for (band = 0; band < bands; ++band)
                        dstData[band][dstInd] = (unsigned char)
                             ((srcData[band][srcInd] * (1 - pRCol) * (1 - pRRow) +
                               srcData[band][srcInd + srcXSize] * (1 - pRCol) * pRRow +
                               srcData[band][srcInd + 1] * pRCol * (1 - pRRow) +
                               srcData[band][srcInd + srcXSize + 1] * pRCol * pRRow) + 0.5);
                }
            }
        }
    }

    for (band = 0; band < bands; ++band)
        dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, 0, 0, dstXSize, dstYSize,
                                                 dstData[band],
                                                 dstXSize, dstYSize, GDT_Byte, 0, 0);

    delete transformer;
    delete[] pxlData;

    if (mAddLogo)
    {
        logoImage logo;
        dstXSize -= logo.width;
        dstYSize -= logo.height;
        if (bands >= 3)
        {
            bands = 3;
            for (band = 0; band < bands; ++band)
                dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, dstXSize, dstYSize, logo.width, logo.height,
                                                         (unsigned char*)&logo.data[band][0],
                                                         logo.width, logo.height, GDT_Byte, 0, 0);
        }
    }

    GDALClose(dstDS);

    mProgressMax = 0;
    mProgressVal = 0;

    return SUCCESS;
}

unsigned int msumr::MSUProj::srcXSize() const
{
    if (mSrcDS)
        return mSrcDS->GetRasterXSize();
    else
        return 0;
}

unsigned int msumr::MSUProj::srcYSize() const
{
    if (mSrcDS)
        return mSrcDS->GetRasterYSize();
    else
        return 0;
}

bool msumr::MSUProj::ifAddLogo() const
{
    return mAddLogo;
}

void msumr::MSUProj::setAddLogo(bool enabled)
{
    mAddLogo = enabled;
}

unsigned int *msumr::MSUProj::progressMaxPtr()
{
    return &mProgressMax;
}

unsigned int *msumr::MSUProj::progressValPtr()
{
    return &mProgressVal;
}

msumr::GCPGrid *msumr::MSUProj::gcpGrid() const
{
    return mGcpGrid;
}
