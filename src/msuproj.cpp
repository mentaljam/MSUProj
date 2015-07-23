#include "msuproj.h"
#include "logoimage.hpp"
#include <msuproj_version.h>
#include <ogrsf_frmts.h>
#include <fstream>
#include <sstream>

MSUMR::MSUProj::MSUProj() :
    srcDS(NULL),
    dstDS(NULL),
    gcps(NULL),
    dstFile(),
    dstFormat("GTiff"),
    hemisphere(true),
    zone(0),
    perimSize(19),
    srcXSize(0),
    srcYSize(0),
    srcSize(0),
    gcpXSize(0),
    gcpYSize(0),
    gcpXStep(0),
    gcpYStep(0),
    gcpSize(0),
    geoTransform(new double[6])
{}

MSUMR::MSUProj::~MSUProj()
{
    delete[] geoTransform;
    if (gcpSize > 0)
        delete[] gcps;
}

const char *MSUMR::MSUProj::getVersion(const unsigned int &type) const
{
    if (type == 0)
        return VERSION_MSUPROJ;
    else if (type == 1)
        return VER_DATE_MSUPROJ;
    else if (type == 2)
        return VER_ARCH_MSUPROJ;
    else
        return "";
}

void MSUMR::MSUProj::setDST(std::string file)
{
    dstFile = file;
}

void MSUMR::MSUProj::setDSTFormat(std::string format)
{
    dstFormat = format;
}

const MSUMR::retCode MSUMR::MSUProj::setSRC(std::string file)
{
    if (srcDS)
        GDALClose(srcDS);
    srcDS = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
    if (!srcDS)
        return errSRC;

    srcXSize = srcDS->GetRasterXSize();
    srcYSize = srcDS->GetRasterYSize();
    srcSize = srcXSize * srcYSize;

    return success;
}

const MSUMR::retCode MSUMR::MSUProj::readGCP(std::string file)
{
    if (!srcDS)
        return errSRC;

    std::ifstream srcGcp(file);
    if (srcGcp.fail())
        return errGCP;

    std::string tmp, line;

    gcpSize = 0;
    while(getline(srcGcp, line))
        ++gcpSize;
    srcGcp.clear();
    srcGcp.seekg(0);

    if (gcpSize)
        delete[] gcps;
    gcps = new gcp[gcpSize];
    unsigned int gIter = 0;
    while(getline(srcGcp, line))
    {
        std::stringstream iss(line);
        getline(iss, tmp, ' ');
        gcps[gIter].x = stoi(tmp);
        getline(iss, tmp, ' ');
        gcps[gIter].y = stoi(tmp);
        getline(iss, tmp, ' ');
        tmp = comma2dot(tmp);
        gcps[gIter].lat = stod(tmp);
        getline(iss, tmp);
        tmp = comma2dot(tmp);
        gcps[gIter].lon = stod(tmp);
        ++gIter;
    }

    gcpXSize = 1;
    while (gcps[gcpXSize].x != 0)
        ++gcpXSize;
    gcpYSize = gcpSize / gcpXSize;
    gcpXStep = srcXSize / gcpXSize + 1;
    gcpYStep = srcYSize / gcpYSize + 1;

    zone = ((int)(((int)((gcps[0].lon + gcps[gcpXSize - 1].lon +
           gcps[gcpSize - gcpXSize].lon + gcps[gcpSize - 1].lon) / 4 + 0.5) + 180) / 6) + 1);
    hemisphere = ((int)((gcps[0].lat + gcps[gcpXSize - 1].lat +
           gcps[gcpSize - gcpXSize].lat + gcps[gcpSize - 1].lat) / 4 + 0.5) > 0);

    return success;
}

void MSUMR::MSUProj::setPerimSize(const unsigned int &perim)
{
    if (perim > 0)
        perimSize = 2 * perim + 1;
}

const std::string MSUMR::MSUProj::getUTM() const
{
    if (!gcpSize)
        return "unknownZone";
    else
    {
        std::string UTM;
        if (hemisphere)
            UTM = "N";
        else
            UTM = "S";
        UTM += std::to_string(zone);

        return UTM;
    }
}

const MSUMR::retCode MSUMR::MSUProj::warp(const bool &useUtm, const bool &zerosAsND)
{

    if (!srcDS)
        return errSRC;

    if (!gcpSize || !gcps)
        return errGCP;

    if (dstFile.empty())
        return errDST;

    unsigned short bands = srcDS->GetRasterCount();

    gcp *gcpsW = new gcp[gcpSize];
    memcpy(gcpsW, gcps, gcpSize * sizeof(gcp));

    OGRSpatialReference latlonSRS;
    latlonSRS.SetWellKnownGeogCS("WGS84");
    char *srsWKT;
    if (useUtm)
    {
        OGRSpatialReference utmSRS;
        utmSRS.SetWellKnownGeogCS("WGS84");
        utmSRS.SetUTM(zone, hemisphere);
        utmSRS.exportToWkt(&srsWKT);
        OGRCoordinateTransformation *transFunc;
        transFunc = OGRCreateCoordinateTransformation(&latlonSRS, &utmSRS);
        for (unsigned int gIter = 0; gIter < gcpSize; ++gIter)
            transFunc->Transform(1, &gcpsW[gIter].lon, &gcpsW[gIter].lat);
        geoTransform[1] = 1000;
        geoTransform[5] = -1000;
    }
    else
    {
        latlonSRS.exportToWkt(&srsWKT);
        geoTransform[1] = 0.01;
        geoTransform[5] = -0.01;
    }

    double coords[4];
    coords[minLON] = gcpsW[0].lon;
    coords[maxLON] = gcpsW[0].lon;
    coords[minLAT] = gcpsW[0].lat;
    coords[maxLAT] = gcpsW[0].lat;
    for (unsigned int i = 1; i < gcpSize; ++i)
    {
        if (coords[minLON] > gcpsW[i].lon)
            coords[minLON] = gcpsW[i].lon;
        if (coords[maxLON] < gcpsW[i].lon)
            coords[maxLON] = gcpsW[i].lon;
        if (coords[minLAT] > gcpsW[i].lat)
            coords[minLAT] = gcpsW[i].lat;
        if (coords[maxLAT] < gcpsW[i].lat)
            coords[maxLAT] = gcpsW[i].lat;
    }

    unsigned int dstXSize = (int)((coords[maxLON] - coords[minLON]) / geoTransform[1] + 0.5);
    unsigned int dstYSize = (int)((coords[maxLAT] - coords[minLAT]) / geoTransform[1] + 0.5);
    unsigned int dstSize = dstXSize * dstYSize;

    geoTransform[0] = coords[minLON];
    geoTransform[2] = 0;
    geoTransform[3] = coords[maxLAT];
    geoTransform[4] = 0;

    unsigned short band;
    GDALDriver *dstDriver;
    dstDriver = GetGDALDriverManager()->GetDriverByName(dstFormat.c_str());
    char **dstOptions = NULL;
    char **dstMetadata = NULL;
    if (dstFormat == "GTiff")
    {
        dstOptions = CSLSetNameValue(dstOptions, "PHOTOMETRIC", "RGB");
        dstOptions = CSLSetNameValue(dstOptions, "COMPRESS", "JPEG");
        dstOptions = CSLSetNameValue(dstOptions, "JPEG_QUALITY", "100");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_IMAGEDESCRIPTION", "Meteor-M MSU-MR georeferenced image");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_SOFTWARE", "MSUProj v" VERSION_MSUPROJ);
    }
    dstDS = dstDriver->Create(dstFile.c_str(), dstXSize, dstYSize, bands, GDT_Byte, dstOptions);
    dstDS->SetMetadata(dstMetadata);
    dstDS->SetProjection(srsWKT);
    dstDS->SetGeoTransform(geoTransform);
    if (zerosAsND)
        for (band = 1; band <= bands; ++band)
            dstDS->GetRasterBand(band)->SetNoDataValue(0);

    unsigned char **srcData = new unsigned char*[bands];
    unsigned char **tmpData = new unsigned char*[bands];

    for (band = 0; band < bands; ++band)
    {
        srcData[band] = new unsigned char[srcSize];
        srcDS->GetRasterBand(band + 1)->RasterIO(GF_Read, 0, 0, srcXSize, srcYSize, srcData[band],
                                                 srcXSize, srcYSize, GDT_Byte, 0, 0);
        tmpData[band] = new unsigned char[dstSize]();
    }

    unsigned int pLine, pRow, pCount, gRow, gCount, dCount,
                 bilDelim = gcpXStep * gcpYStep;
    int x, y, xx, yy;
    double lat, lon;

    for (pLine = 0; pLine < srcYSize; ++pLine)
    {
        unsigned int gLine = (int)(pLine / gcpYStep);
        pCount = pLine * srcXSize;
        for (pRow = 0; pRow < srcXSize; ++pRow, ++pCount)
        {
            gRow = (int)(pRow / gcpXStep);
            gCount = gRow + gLine * gcpXSize;

            x = pRow - gRow * gcpXStep;
            xx = gcpXStep - x;
            y = pLine - gLine * gcpYStep;
            yy = gcpYStep - y;

            lon = (gcpsW[gCount].lon * xx * yy +
                   gcpsW[gCount + 1].lon * x * yy +
                   gcpsW[gCount + gcpXSize].lon * xx * y +
                   gcpsW[gCount + gcpXSize + 1].lon * x * y) / bilDelim;
            lat = (gcpsW[gCount].lat * xx * yy +
                   gcpsW[gCount + 1].lat * x * yy +
                   gcpsW[gCount + gcpXSize].lat * xx * y +
                   gcpsW[gCount + gcpXSize + 1].lat * x * y) / bilDelim;

            lon = (lon - geoTransform[0]) / geoTransform[1];
            lat = (lat - geoTransform[3]) / geoTransform[5];

            dCount = (int)(lat + 0.5) * dstXSize + (int)(lon + 0.5);

            for (band = 0; band < bands; ++band)
            {
                if (tmpData[band][dCount] == 0)
                    tmpData[band][dCount] = srcData[band][pCount];
                else
                    tmpData[band][dCount] = (unsigned char)((tmpData[band][dCount] + srcData[band][pCount]) / 2 + 0.5);
            }
        }
    }

    unsigned char **dstData = new unsigned char*[bands];
    for (band = 0; band < bands; ++band)
    {
        delete[] srcData[band];
        dstData[band] = new unsigned char[dstSize]();
    }
    delete[] srcData;

    double vl, wDelim;
    double *sum = new double[bands];

    for (pLine = 0; pLine < dstYSize; ++pLine)
    {
        pCount = pLine * dstXSize;
        for (pRow = 0; pRow < dstXSize; ++pRow, ++pCount)
        {
            x = pRow;
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

            for (dCount = 2; dCount < perimSize; dCount += 2)
            {
                --y;
                yy = y * dstXSize + x;
                if (yy >= 0 && yy < dstSize &&
                    tmpData[0][yy] > 0)
                {
                    vl = sqrt(pow(pRow - x, 2) + pow(pLine - y, 2));
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
                        vl = sqrt(pow(pRow - x, 2) + pow(pLine - y, 2));
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
                        vl = sqrt(pow(pRow - x, 2) + pow(pLine - y, 2));
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
                        vl = sqrt(pow(pRow - x, 2) + pow(pLine - y, 2));
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
                        vl = sqrt(pow(pRow - x, 2) + pow(pLine - y, 2));
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
                dstData[band][pCount] = (unsigned char)(sum[band] / wDelim + 0.5);
        }
    }

    delete[] gcpsW;
    delete[] sum;

    for (band = 0; band < bands; ++band)
    {
        dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, 0, 0, dstXSize, dstYSize, dstData[band],
                                                 dstXSize, dstYSize, GDT_Byte, 0, 0);
        delete[] tmpData[band];
        delete[] dstData[band];
    }
    delete[] tmpData;
    delete[] dstData;

    logoImage logo;
    dstXSize -= logo.width;
    dstYSize -= logo.height;
    if (bands >= 3)
    {
        bands = 3;
        for (band = 0; band < bands; ++band)
            dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, dstXSize, dstYSize,
                                                     logo.width, logo.height, (unsigned char*)&logo.data[band][0],
                                                     logo.width, logo.height, GDT_Byte, 0, 0);
    }

    GDALClose(dstDS);

    return success;
}

unsigned int MSUMR::MSUProj::getSrcYSize() const
{
    return srcYSize;
}
unsigned int MSUMR::MSUProj::getGcpYStep() const
{
    return gcpYStep;
}

unsigned int MSUMR::MSUProj::getGcpXStep() const
{
    return gcpXStep;
}

unsigned int MSUMR::MSUProj::getGcpYSize() const
{
    return gcpYSize;
}

unsigned int MSUMR::MSUProj::getGcpXSize() const
{
    return gcpXSize;
}


unsigned int MSUMR::MSUProj::getSrcXSize() const
{
    return srcXSize;
}


std::string MSUMR::MSUProj::comma2dot(std::string str) const
{
    unsigned int size = str.size();
    for (unsigned int i = 0; i < size; ++i)
        if (str[i] == ',')
            str[i] = '.';

    return str;
}
