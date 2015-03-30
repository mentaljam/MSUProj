#include "msugeo.h"
#include "logomark.hpp"
#include "info.h"
#include <ogrsf_frmts.h>
#include <fstream>
#include <sstream>

msumr::msugeo::msugeo()
{
    ifUTM = false;

    srcXSize = 0;
    srcYSize = 0;
    srcSize  = 0;

    dstXSize = 0;
    dstYSize = 0;
    dstSize  = 0;

    gcpXSize = 0;
    gcpYSize = 0;
    gcpXStep = 0;
    gcpYStep = 0;
    gcpSize  = 0;

    geoTransform = new double[6];

    perimSize = 19;

    zone = 0;
    hemisphere = 0;

    srcDS = NULL;
    dstFile = NULL;
    dstDS = NULL;

    dstFormat = (char*)"GTiff";
}

msumr::msugeo::~msugeo()
{
    delete[] geoTransform;
    if (gcpSize > 0)
        delete[] gcps;
    if (srcDS != NULL)
        srcDS->~GDALDataset();
    if (dstDS != NULL)
        dstDS->~GDALDataset();
}

const char *msumr::msugeo::getVersion()
{
    return VERSION;
}

void msumr::msugeo::setDST(const char *file)
{
    dstFile = (char*)file;
}

void msumr::msugeo::setDSTFormat(const char *format)
{
    dstFormat = (char*)format;
}

msumr::retCode msumr::msugeo::setSRC(const char *file)
{
    srcDS = (GDALDataset*)GDALOpen(file, GA_ReadOnly);
    if (srcDS == NULL)
        return errSRC;

    srcXSize = srcDS->GetRasterXSize();
    srcYSize = srcDS->GetRasterYSize();
    srcSize = srcXSize * srcYSize;

    return success;
}

msumr::retCode msumr::msugeo::readGCP(const char *file)
{
    if (srcDS == NULL)
        return errSRC;

    std::ifstream srcGcp(file);
    if (srcGcp.fail())
        return errGCP;

    std::string tmp, line;

    while(getline(srcGcp, line))
        ++gcpSize;
    srcGcp.clear();
    srcGcp.seekg(0);

    gcps = new gcp[gcpSize];
    int i = 0;
    while(getline(srcGcp, line))
    {
        std::stringstream iss(line);
        getline(iss, tmp, ' ');
        gcps[i].x = stoi(tmp);
        getline(iss, tmp, ' ');
        gcps[i].y = stoi(tmp);
        getline(iss, tmp, ' ');
        tmp = comma2dot(tmp);
        gcps[i].lat = stod(tmp);
        getline(iss, tmp);
        tmp = comma2dot(tmp);
        gcps[i].lon = stod(tmp);
        ++i;
    }

    gcpXSize = 1;
    while (gcps[gcpXSize].x != 0)
        ++gcpXSize;
    gcpYSize = gcpSize / gcpXSize;
    gcpXStep = srcXSize / gcpXSize + 1;
    gcpYStep = srcYSize / gcpYSize + 1;

    zone = ((int)((gcps[(int)(gcpSize / 2)].lon + 180) / 6) + 1);
    hemisphere = (gcps[(int)(gcpSize / 2)].lat > 0);

    return success;
}

void msumr::msugeo::setPerimSize(int perim)
{
    if (perim > 0)
        perimSize = 2 * perim + 1;
}

void msumr::msugeo::useUTM(bool use)
{
    ifUTM = use;
}

msumr::retCode msumr::msugeo::warp()
{
    if (srcDS == NULL)
        return errSRC;

    if (gcps == NULL)
        return errGCP;

    if (dstFile == NULL)
        return errDST;

    gcp *gcpsW = new gcp[gcpSize];
    memcpy(gcpsW, gcps, gcpSize * sizeof(gcp));

    OGRSpatialReference latlonSRS;
    latlonSRS.SetWellKnownGeogCS("WGS84");
    char *srsWKT;
    if (ifUTM)
    {
        OGRSpatialReference utmSRS;
        utmSRS.SetWellKnownGeogCS("WGS84");
        utmSRS.SetUTM(zone, hemisphere);
        utmSRS.exportToWkt(&srsWKT);
        OGRCoordinateTransformation *transFunc;
        transFunc = OGRCreateCoordinateTransformation(&latlonSRS, &utmSRS);
        for (int g = 0; g < gcpSize; ++g)
            transFunc->Transform(1, &gcpsW[g].lon, &gcpsW[g].lat);
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
    for (int i = 1; i < gcpSize; ++i)
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

    dstXSize = (int)((coords[maxLON] - coords[minLON]) / geoTransform[1] + 0.5);
    dstYSize = (int)((coords[maxLAT] - coords[minLAT]) / geoTransform[1] + 0.5);
    dstSize = dstXSize * dstYSize;

    geoTransform[0] = coords[minLON];
    geoTransform[2] = 0;
    geoTransform[3] = coords[maxLAT];
    geoTransform[4] = 0;

    GDALDriver *dstDriver;
    dstDriver = GetGDALDriverManager()->GetDriverByName(dstFormat);
    char **dstOptions = NULL;
    char **dstMetadata = NULL;
    if (strcmp(dstFormat, "GTiff") == 0)
    {
        dstOptions = CSLSetNameValue(dstOptions, "PHOTOMETRIC", "RGB");
        dstOptions = CSLSetNameValue(dstOptions, "COMPRESS", "JPEG");
        dstOptions = CSLSetNameValue(dstOptions, "JPEG_QUALITY", "100");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_IMAGEDESCRIPTION", "Meteor-M MSU-MR georeferenced image");
        dstMetadata = CSLSetNameValue(dstMetadata, "TIFFTAG_SOFTWARE", "msugeo v" VERSION);
    }
    dstDS = dstDriver->Create(dstFile, dstXSize, dstYSize, 3, GDT_Byte, dstOptions);
    dstDS->SetMetadata(dstMetadata);
    dstDS->SetProjection(srsWKT);
    dstDS->SetGeoTransform(geoTransform);

    int band;
    unsigned char *srcData[3];
    unsigned char *tmpData[3];

    for (band = 0; band < 3; ++band)
    {
        srcData[band] = new unsigned char[srcSize];
        srcDS->GetRasterBand(band + 1)->RasterIO(GF_Read, 0, 0, srcXSize, srcYSize, srcData[band],
                                                 srcXSize, srcYSize, GDT_Byte, 0, 0);
        tmpData[band] = new unsigned char[dstSize]();
    }

    int pLine, pRow, pCount, gLine, gRow, gCount,
        x, y, xx, yy, dCount;
    double lat, lon;
    int bilDelim = gcpXStep * gcpYStep;

    for (pLine = 0; pLine < srcYSize; ++pLine)
    {
        gLine = (int)(pLine / gcpYStep);
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

            for (band = 0; band < 3; ++band)
            {
                if (tmpData[band][dCount] == 0)
                    tmpData[band][dCount] = srcData[band][pCount];
                else
                    tmpData[band][dCount] = (unsigned char)((tmpData[band][dCount] + srcData[band][pCount]) / 2 + 0.5);
            }
        }
    }

    unsigned char *dstData[3];
    for (band = 0; band < 3; ++band)
    {
        delete[] srcData[band];
        dstData[band] = new unsigned char[dstSize]();
    }

    double vl, wDelim, sum[3];

    for (pLine = 0; pLine < dstYSize; ++pLine)
    {
        pCount = pLine * dstXSize;
        for (pRow = 0; pRow < dstXSize; ++pRow, ++pCount)
        {
            x = pRow;
            y = pLine;
            sum[0] = tmpData[0][pCount];
            sum[1] = tmpData[1][pCount];
            sum[2] = tmpData[2][pCount];

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
                    sum[0] += tmpData[0][yy] / vl;
                    sum[1] += tmpData[1][yy] / vl;
                    sum[2] += tmpData[2][yy] / vl;
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
                        sum[0] += tmpData[0][yy] / vl;
                        sum[1] += tmpData[1][yy] / vl;
                        sum[2] += tmpData[2][yy] / vl;
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
                        sum[0] += tmpData[0][yy] / vl;
                        sum[1] += tmpData[1][yy] / vl;
                        sum[2] += tmpData[2][yy] / vl;
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
                        sum[0] += tmpData[0][yy] / vl;
                        sum[1] += tmpData[1][yy] / vl;
                        sum[2] += tmpData[2][yy] / vl;
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
                        sum[0] += tmpData[0][yy] / vl;
                        sum[1] += tmpData[1][yy] / vl;
                        sum[2] += tmpData[2][yy] / vl;
                        wDelim += 1 / vl;
                        ++xx;
                    }
                }

                if (xx >= dCount * 4)
                    break;
            }
            dstData[0][pCount] = (unsigned char)(sum[0] / wDelim + 0.5);
            dstData[1][pCount] = (unsigned char)(sum[1] / wDelim + 0.5);
            dstData[2][pCount] = (unsigned char)(sum[2] / wDelim + 0.5);
        }
    }

    delete[] gcpsW;

    for (band = 0; band < 3; ++band)
    {
        dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, 0, 0, dstXSize, dstYSize, dstData[band],
                                                 dstXSize, dstYSize, GDT_Byte, 0, 0);
        delete[] tmpData[band];
        delete[] dstData[band];
    }

    logomark logo;
    dstXSize -= logo.width;
    dstYSize -= logo.height;
    dstDS->GetRasterBand(1)->RasterIO(GF_Write, dstXSize, dstYSize,
                                      logo.width, logo.height, (unsigned char*)&logo.data[0],
                                      logo.width, logo.height, GDT_Byte, 0, 0);
    dstDS->GetRasterBand(2)->RasterIO(GF_Write, dstXSize, dstYSize,
                                      logo.width, logo.height, (unsigned char*)&logo.data[logo.width * logo.height],
                                      logo.width, logo.height, GDT_Byte, 0, 0);
    dstDS->GetRasterBand(3)->RasterIO(GF_Write, dstXSize, dstYSize,
                                      logo.width, logo.height, (unsigned char*)&logo.data[logo.width * logo.height * 2],
                                      logo.width, logo.height, GDT_Byte, 0, 0);

    return success;
}

std::string msumr::msugeo::comma2dot(std::string str)
{
    for (int i = 0; i < str.size(); ++i)
        if (str[i] == ',')
            str[i] = '.';

    return str;
}