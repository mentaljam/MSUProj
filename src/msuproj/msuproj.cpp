#include <msuproj.h>
#include <logoimage.hpp>
#include <ogrsf_frmts.h>
#include <fstream>
#include <sstream>

msumr::MSUProj::MSUProj() :
    mSrcDS(NULL),
    mGCPs(NULL),
    mDstFile(),
    mDstFormat("GTiff"),
    mAddLogo(true),
    mHemisphere(true),
    mZone(0),
    mPerimSize(19),
    mGCPXSize(0),
    mGCPYSize(0),
    mGCPXStep(0),
    mGCPYStep(0),
    mGCPSize(0),
    mGeoTransform(new double[6])
{}

msumr::MSUProj::~MSUProj()
{
    delete[] mGeoTransform;
    if (mGCPSize > 0)
        delete[] mGCPs;
}

const char *msumr::MSUProj::getVersion(VERSION_TYPE type) const
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

    std::ifstream srcGCP(file);
    if (srcGCP.fail())
        return ERROR_GCP;

    std::string tmp, line;

    mGCPSize = 0;
    while(getline(srcGCP, line))
        ++mGCPSize;
    srcGCP.clear();
    srcGCP.seekg(0);

    if (mGCPSize)
        delete[] mGCPs;
    mGCPs = new GCP[mGCPSize];
    unsigned int gIter = 0;
    while(getline(srcGCP, line))
    {
        std::stringstream iss(line);
        getline(iss, tmp, ' ');
        mGCPs[gIter].x = stoi(tmp);
        getline(iss, tmp, ' ');
        mGCPs[gIter].y = stoi(tmp);
        getline(iss, tmp, ' ');
        tmp = comma2dot(tmp);
        std::stringstream(tmp) >> mGCPs[gIter].lat;
//        mGCPs[gIter].lat = stod(tmp);
        getline(iss, tmp);
        tmp = comma2dot(tmp);
        std::stringstream(tmp) >> mGCPs[gIter].lon;
//        mGCPs[gIter].lon = stod(tmp);
        ++gIter;
    }

    mGCPXSize = 1;
    while (mGCPs[mGCPXSize].x != 0)
        ++mGCPXSize;
    mGCPYSize = mGCPSize / mGCPXSize;
    mGCPXStep = mSrcDS->GetRasterXSize() / mGCPXSize + 1;
    mGCPYStep = mSrcDS->GetRasterYSize() / mGCPYSize + 1;

    mZone = ((int)(((int)((mGCPs[0].lon + mGCPs[mGCPXSize - 1].lon +
           mGCPs[mGCPSize - mGCPXSize].lon + mGCPs[mGCPSize - 1].lon) / 4 + 0.5) + 180) / 6) + 1);
    mHemisphere = ((int)((mGCPs[0].lat + mGCPs[mGCPXSize - 1].lat +
           mGCPs[mGCPSize - mGCPXSize].lat + mGCPs[mGCPSize - 1].lat) / 4 + 0.5) > 0);

    return SUCCESS;
}

void msumr::MSUProj::setPerimSize(const unsigned int &perim)
{
    if (perim > 0)
        mPerimSize = 2 * perim + 1;
}

const std::string msumr::MSUProj::getUTM() const
{
    if (!mGCPSize)
        return "unknownZone";
    else
    {
        std::string UTM;
        if (mHemisphere)
            UTM = "N";
        else
            UTM = "S";
        UTM += std::to_string(mZone);

        return UTM;
    }
}

const msumr::RETURN_CODE msumr::MSUProj::warp(const bool &useUtm, const bool &zerosAsND)
{

    if (!mSrcDS)
        return ERROR_SRC;

    if (!mGCPSize || !mGCPs)
        return ERROR_GCP;

    if (mDstFile.empty())
        return ERROR_DST;

    unsigned short bands = mSrcDS->GetRasterCount();

    GCP *GCPsW = new GCP[mGCPSize];
    memcpy(GCPsW, mGCPs, mGCPSize * sizeof(GCP));

    OGRSpatialReference latlonSRS;
    latlonSRS.SetWellKnownGeogCS("WGS84");
    char *srsWKT;
    if (useUtm)
    {
        OGRSpatialReference utmSRS;
        utmSRS.SetWellKnownGeogCS("WGS84");
        utmSRS.SetUTM(mZone, mHemisphere);
        utmSRS.exportToWkt(&srsWKT);
        OGRCoordinateTransformation *transFunc;
        transFunc = OGRCreateCoordinateTransformation(&latlonSRS, &utmSRS);
        for (unsigned int gIter = 0; gIter < mGCPSize; ++gIter)
            transFunc->Transform(1, &GCPsW[gIter].lon, &GCPsW[gIter].lat);
        mGeoTransform[1] = 1000;
        mGeoTransform[5] = -1000;
    }
    else
    {
        latlonSRS.exportToWkt(&srsWKT);
        mGeoTransform[1] = 0.01;
        mGeoTransform[5] = -0.01;
    }

    double coords[4];
    coords[MIN_LON] = GCPsW[0].lon;
    coords[MAX_LON] = GCPsW[0].lon;
    coords[MIN_LAT] = GCPsW[0].lat;
    coords[MAX_LAT] = GCPsW[0].lat;
    for (unsigned int i = 1; i < mGCPSize; ++i)
    {
        if (coords[MIN_LON] > GCPsW[i].lon)
            coords[MIN_LON] = GCPsW[i].lon;
        if (coords[MAX_LON] < GCPsW[i].lon)
            coords[MAX_LON] = GCPsW[i].lon;
        if (coords[MIN_LAT] > GCPsW[i].lat)
            coords[MIN_LAT] = GCPsW[i].lat;
        if (coords[MAX_LAT] < GCPsW[i].lat)
            coords[MAX_LAT] = GCPsW[i].lat;
    }

    unsigned int dstXSize = (int)((coords[MAX_LON] - coords[MIN_LON]) / mGeoTransform[1] + 0.5);
    unsigned int dstYSize = (int)((coords[MAX_LAT] - coords[MIN_LAT]) / mGeoTransform[1] + 0.5);
    unsigned int dstSize = dstXSize * dstYSize;

    mGeoTransform[0] = coords[MIN_LON];
    mGeoTransform[2] = 0;
    mGeoTransform[3] = coords[MAX_LAT];
    mGeoTransform[4] = 0;

    unsigned short band;
    GDALDriver *dstDriver;
    dstDriver = GetGDALDriverManager()->GetDriverByName(mDstFormat.c_str());
    char **dstOptions = NULL;
    char **dstMetadata = NULL;
    if (mDstFormat == "GTiff")
    {
        dstOptions = CSLSetNameValue(dstOptions, "PHOTOMETRIC", "RGB");
        dstOptions = CSLSetNameValue(dstOptions, "COMPRESS", "JPEG");
        dstOptions = CSLSetNameValue(dstOptions, "JPEG_QUALITY", "100");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_IMAGEDESCRIPTION", "Meteor-M MSU-MR georeferenced image");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_SOFTWARE", "MSUProj v" VERSION_MSUPROJ_STRING);
    }
    GDALDataset *dstDS = dstDriver->Create(mDstFile.c_str(), dstXSize, dstYSize, bands, GDT_Byte, dstOptions);
    if (dstDS == NULL)
        return ERROR_DST;

    dstDS->SetMetadata(dstMetadata);
    dstDS->SetProjection(srsWKT);
    dstDS->SetGeoTransform(mGeoTransform);
    if (zerosAsND)
        for (band = 1; band <= bands; ++band)
            dstDS->GetRasterBand(band)->SetNoDataValue(0);

    unsigned char **pxsData = new unsigned char*[bands];
    unsigned char **tmpData = new unsigned char*[bands];

    unsigned int srcXSize = mSrcDS->GetRasterXSize(),
                 srcYSize = mSrcDS->GetRasterYSize();
    unsigned int srcSize  = srcXSize * srcYSize;

    unsigned int pxsSize  = dstSize;
    if (pxsSize < srcSize)
        pxsSize = srcSize;

    for (band = 0; band < bands; ++band)
    {
        pxsData[band] = new unsigned char[pxsSize];
        mSrcDS->GetRasterBand(band + 1)->RasterIO(GF_Read, 0, 0, srcXSize, srcYSize, pxsData[band],
                                                  srcXSize, srcYSize, GDT_Byte, 0, 0);
        tmpData[band] = new unsigned char[dstSize]();
    }

    unsigned int pLine, pColumn, pCount, gColumn, gCount, dCount,
                 bilDelim = mGCPXStep * mGCPYStep;
    int x, y, xx, yy;
    double lat, lon;

    for (pLine = 0; pLine < srcYSize; ++pLine)
    {
        unsigned int gLine = (int)(pLine / mGCPYStep);
        pCount = pLine * srcXSize;
        for (pColumn = 0; pColumn < srcXSize; ++pColumn, ++pCount)
        {
            gColumn = (int)(pColumn / mGCPXStep);
            gCount = gColumn + gLine * mGCPXSize;

            x  = pColumn - gColumn * mGCPXStep;
            xx = mGCPXStep - x;
            y  = pLine - gLine * mGCPYStep;
            yy = mGCPYStep - y;

            lon = (GCPsW[gCount].lon * xx * yy +
                   GCPsW[gCount + 1].lon * x * yy +
                   GCPsW[gCount + mGCPXSize].lon * xx * y +
                   GCPsW[gCount + mGCPXSize + 1].lon * x * y) / bilDelim;
            lat = (GCPsW[gCount].lat * xx * yy +
                   GCPsW[gCount + 1].lat * x * yy +
                   GCPsW[gCount + mGCPXSize].lat * xx * y +
                   GCPsW[gCount + mGCPXSize + 1].lat * x * y) / bilDelim;

            lon = (lon - mGeoTransform[0]) / mGeoTransform[1];
            lat = (lat - mGeoTransform[3]) / mGeoTransform[5];

            dCount = (int)(lat + 0.5) * dstXSize + (int)(lon + 0.5);

            for (band = 0; band < bands; ++band)
            {
                if (tmpData[band][dCount] == 0)
                    tmpData[band][dCount] = pxsData[band][pCount];
                else
                    tmpData[band][dCount] = (unsigned char)((tmpData[band][dCount] + pxsData[band][pCount]) / 2 + 0.5);
            }
        }
    }

    for (band = 0; band < bands; ++band)
        memset(pxsData[band], 0, pxsSize);

    double vl, wDelim;
    double *sum = new double[bands];

    for (pLine = 0; pLine < dstYSize; ++pLine)
    {
        pCount = pLine * dstXSize;
        for (pColumn = 0; pColumn < dstXSize; ++pColumn, ++pCount)
        {
            x = pColumn;
            y = pLine;
            for (band = 0; band < bands; ++band)
                sum[band] = tmpData[band][pCount];

            if (tmpData[0][pCount] == 0)
            {
                wDelim = 0;
                xx = 0;
            }
            else
            {
                wDelim = 1;
                xx = 1;
            }

            for (dCount = 2; dCount < mPerimSize; dCount += 2)
            {
                --y;
                yy = y * dstXSize + x;
                if (yy >= 0 && yy < dstSize &&
                    tmpData[0][yy] > 0)
                {
                    vl = sqrt(pow(pColumn - x, 2) + pow(pLine - y, 2));
                    for (band = 0; band < bands; ++band)
                        sum[band] += tmpData[band][yy] / vl;
                    wDelim += 1 / vl;
                    ++xx;
                }

                for (gCount = 1; gCount < dCount; ++gCount)
                {
                    --x;
                    yy = y * dstXSize + x;
                    if (yy >= 0 && yy < dstSize &&
                        tmpData[0][yy] > 0)
                    {
                        vl = sqrt(pow(pColumn - x, 2) + pow(pLine - y, 2));
                        for (band = 0; band < bands; ++band)
                            sum[band] += tmpData[band][yy] / vl;
                        wDelim += 1 / vl;
                        ++xx;
                    }
                }

                for (gCount = 1; gCount <= dCount; ++gCount)
                {
                    ++y;
                    yy = y * dstXSize + x;
                    if (yy >= 0 && yy < dstSize &&
                        tmpData[0][yy] > 0)
                    {
                        vl = sqrt(pow(pColumn - x, 2) + pow(pLine - y, 2));
                        for (band = 0; band < bands; ++band)
                            sum[band] += tmpData[band][yy] / vl;
                        wDelim += 1 / vl;
                        ++xx;
                    }
                }

                for (gCount = 1; gCount <= dCount; ++gCount)
                {
                    ++x;
                    yy = y * dstXSize + x;
                    if (yy >= 0 && yy < dstSize &&
                        tmpData[0][yy] > 0)
                    {
                        vl = sqrt(pow(pColumn - x, 2) + pow(pLine - y, 2));
                        for (band = 0; band < bands; ++band)
                            sum[band] += tmpData[band][yy] / vl;
                        wDelim += 1 / vl;
                        ++xx;
                    }
                }

                for (gCount = 1; gCount <= dCount; ++gCount)
                {
                    --y;
                    yy = y * dstXSize + x;
                    if (yy >= 0 && yy < dstSize &&
                        tmpData[0][yy] > 0)
                    {
                        vl = sqrt(pow(pColumn - x, 2) + pow(pLine - y, 2));
                        for (band = 0; band < bands; ++band)
                            sum[band] += tmpData[band][yy] / vl;
                        wDelim += 1 / vl;
                        ++xx;
                    }
                }

                if (xx >= dCount * 4)
                    break;
            }
            for (band = 0; band < bands; ++band)
                pxsData[band][pCount] = (unsigned char)(sum[band] / wDelim + 0.5);
        }
    }

    delete[] GCPsW;
    delete[] sum;

    for (band = 0; band < bands; ++band)
    {
        dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, 0, 0, dstXSize, dstYSize, pxsData[band],
                                                 dstXSize, dstYSize, GDT_Byte, 0, 0);
        delete[] tmpData[band];
        delete[] pxsData[band];
    }
    delete[] tmpData;
    delete[] pxsData;

    logoImage logo;
    dstXSize -= logo.width;
    dstYSize -= logo.height;
    if (mAddLogo)
    {
        if (bands >= 3)
        {
            bands = 3;
            for (band = 0; band < bands; ++band)
                dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, dstXSize, dstYSize,
                                                         logo.width, logo.height, (unsigned char*)&logo.data[band][0],
                        logo.width, logo.height, GDT_Byte, 0, 0);
        }
    }

    GDALClose(dstDS);

    return SUCCESS;
}

unsigned int msumr::MSUProj::getSrcXSize() const
{
    if (mSrcDS)
        return mSrcDS->GetRasterXSize();
    else
        return 0;
}

unsigned int msumr::MSUProj::getSrcYSize() const
{
    if (mSrcDS)
        return mSrcDS->GetRasterYSize();
    else
        return 0;
}

unsigned int msumr::MSUProj::getGCPXSize() const
{
    return mGCPXSize;
}

unsigned int msumr::MSUProj::getGCPYSize() const
{
    return mGCPYSize;
}

unsigned int msumr::MSUProj::getGCPXStep() const
{
    return mGCPXStep;
}

unsigned int msumr::MSUProj::getGCPYStep() const
{
    return mGCPYStep;
}

bool msumr::MSUProj::ifAddLogo() const
{
    return mAddLogo;
}

void msumr::MSUProj::setAddLogo(bool enabled)
{
    mAddLogo = enabled;
}
