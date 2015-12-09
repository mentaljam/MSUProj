#include <msuproj.h>
#include <logoimage.hpp>
#include <ogrsf_frmts.h>
#include <fstream>
#include <cfloat>
#include <vector>


msumr::MSUProj::MSUProj() :
    mSrcDS(0),
    mGCPs(0),
    mDstFile(),
    mDstFormat("GTiff"),
    mAddLogo(true),
    mHemisphere(true),
    mZone(0),
    mGCPXSize(0),
    mGCPYSize(0),
    mGCPXStep(0),
    mGCPYStep(0),
    mGCPSize(0),
    mProgressMax(0),
    mProgressVal(0)
{
    // Need this for reading GCP files because Qt sets locale to system
    // (error with decimal delimiter in stod function)
    setlocale(LC_NUMERIC, "C");
}

msumr::MSUProj::~MSUProj()
{
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

    if (mGCPSize)
    {
        mGCPSize = 0;
        delete[] mGCPs;
    }

    std::string line;

    while(getline(srcGCP, line))
        ++mGCPSize;
    srcGCP.clear();
    srcGCP.seekg(0);

    mGCPs = new GCP[mGCPSize];
    unsigned int gcpInd = 0;
    size_t pos;
    while(getline(srcGCP, line))
    {
        mGCPs[gcpInd].x = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        mGCPs[gcpInd].y = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        line = comma2dot(line);

        mGCPs[gcpInd].lat = std::stod(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        mGCPs[gcpInd].lon = std::stod(line, &pos);

        ++gcpInd;
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
    double geoTransform[6];
    if (useUtm)
    {
        OGRSpatialReference utmSRS;
        utmSRS.SetWellKnownGeogCS("WGS84");
        utmSRS.SetUTM(mZone, mHemisphere);
        utmSRS.exportToWkt(&srsWKT);
        OGRCoordinateTransformation *transFunc;
        transFunc = OGRCreateCoordinateTransformation(&latlonSRS, &utmSRS);
        for (unsigned int gcpInd = 0; gcpInd < mGCPSize; ++gcpInd)
            transFunc->Transform(1, &GCPsW[gcpInd].lon, &GCPsW[gcpInd].lat);
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

    unsigned int dstXSize = (int)((coords[MAX_LON] - coords[MIN_LON]) / geoTransform[1] + 0.5);
    unsigned int dstYSize = (int)((coords[MAX_LAT] - coords[MIN_LAT]) / geoTransform[1] + 0.5);
    unsigned int dstSize = dstXSize * dstYSize;

    geoTransform[0] = coords[MIN_LON];
    geoTransform[2] = 0;
    geoTransform[3] = coords[MAX_LAT];
    geoTransform[4] = 0;

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

    // 10x10 GCP squares
    unsigned int qdr10XSize, qrd10YSize;
    qdrNode *qdr10Centers = this->sqSurface(GCPsW, 10, &qdr10XSize, &qrd10YSize);
    unsigned int qrd10Size = qdr10XSize * qrd10YSize;

    // 1x1 GCP squares
    unsigned int qdr1XSize, qdr1YSize;
    qdrNode *qdr1Centers = this->sqSurface(GCPsW, 2, &qdr1XSize, &qdr1YSize);

    // 1x1 GCP triangles
    unsigned int triXSize, triYSize;
    msumr::TriNode *triNodes = this->triSurface(GCPsW, &triXSize, &triYSize);

    // Real image borders inside the output raster
    double *borders      = new double[(dstYSize + dstXSize) * 2]();
    double *borderTop    = &borders[0];
    double *borderRight  = &borders[dstXSize];
    double *borderButton = &borders[dstXSize + dstYSize];
    double *borderLeft   = &borders[dstXSize * 2 + dstYSize];
    this->calculateBorder(BORDER_TOP,    GCPsW, geoTransform, borderTop, dstXSize);
    this->calculateBorder(BORDER_RIGHT,  GCPsW, geoTransform, borderRight, dstXSize);
    this->calculateBorder(BORDER_BUTTON, GCPsW, geoTransform, borderButton, dstYSize);
    this->calculateBorder(BORDER_LEFT,   GCPsW, geoTransform, borderLeft, dstYSize);

    // Required variables
    unsigned int dstInd = 0;
    double difLat, difLon;
    double lat = geoTransform[3];

    // Destination raster lines loop
    for (unsigned int pLine = 0; pLine < dstYSize; ++pLine, ++mProgressVal, lat += geoTransform[5])
    {
        double lon = geoTransform[0];
        // Destination raster columns loop
        for (unsigned int pColumn = 0; pColumn < dstXSize; ++pColumn, ++dstInd, lon += geoTransform[1])
        {
            // Skip pixel if it is not in image borders
            if (!(((borderTop[pColumn]     && lat < borderTop[pColumn])    || !borderTop[pColumn]) &&
                   ((borderButton[pColumn] && lat > borderButton[pColumn]) || !borderButton[pColumn]) &&
                   ((borderRight[pLine]    && lon < borderRight[pLine])    || !borderRight[pLine]) &&
                   ((borderLeft[pLine]     && lon > borderLeft[pLine])     || !borderLeft[pLine])))
                continue;

            // Find near 10x10 square
            unsigned int nearInd = 0;
            double distance = DBL_MAX;
            for (unsigned int qdr10Iter = 0; qdr10Iter < qrd10Size; ++qdr10Iter)
            {
                difLat = qdr10Centers[qdr10Iter].lat - lat;
                difLon = qdr10Centers[qdr10Iter].lon - lon;
                double currentDistance = difLat * difLat + difLon * difLon;
                if (currentDistance < distance)
                {
                    distance = currentDistance;
                    nearInd  = qdr10Iter;
                }
            }

            // Upper left GCP of the near 10x10 square (1x1 square index)
            unsigned int qdr1Row = (int)(qdr10Centers[nearInd].GCP0 / mGCPXSize);
            unsigned int qdr1RowLast = qdr1Row + 8;
            if (qdr1Row > qdr1YSize - 1)
                qdr1Row = qdr1YSize - 1;
            if (qdr1RowLast > qdr1YSize)
                qdr1RowLast = qdr1YSize;
            unsigned int qdr1ColFirst = qdr10Centers[nearInd].GCP0 - mGCPXSize * qdr1Row;
            unsigned int sq1ColLast = qdr1ColFirst + 8;
            if (qdr1ColFirst > qdr1XSize - 1)
                qdr1ColFirst = qdr1XSize - 1;
            if (sq1ColLast > qdr1XSize)
                sq1ColLast = qdr1XSize;

            // Find near GCP
            nearInd = 0;
            distance = DBL_MAX;
            for (qdr1Row; qdr1Row < qdr1RowLast; ++qdr1Row)
            {
                unsigned int gcpInd = qdr1Row * qdr1XSize + qdr1ColFirst;
                for (unsigned int qdr1Col = qdr1ColFirst; qdr1Col < sq1ColLast; ++qdr1Col, ++gcpInd)
                {
                    difLat = qdr1Centers[gcpInd].lat - lat;
                    difLon = qdr1Centers[gcpInd].lon - lon;
                    double currentDistance = difLat * difLat + difLon * difLon;
                    if (currentDistance < distance)
                    {
                        distance = currentDistance;
                        nearInd  = gcpInd;
                    }
                }
            }

            // Calculate pixel position in source data
            double pRRow = triNodes[nearInd].row0 -
                           ((lon - triNodes[nearInd].lon0) * triNodes[nearInd].aRow -
                            (lat - triNodes[nearInd].lat0) * triNodes[nearInd].bRow) /
                           triNodes[nearInd].c;
            double pRCol = triNodes[nearInd].col0 -
                           ((lon - triNodes[nearInd].lon0) * triNodes[nearInd].aCol -
                            (lat - triNodes[nearInd].lat0) * triNodes[nearInd].bCol) /
                           triNodes[nearInd].c;

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

    for (band = 0; band < bands; ++band)
        dstDS->GetRasterBand(band + 1)->RasterIO(GF_Write, 0, 0, dstXSize, dstYSize,
                                                 dstData[band],
                                                 dstXSize, dstYSize, GDT_Byte, 0, 0);

    delete[] borders;
    delete[] triNodes;
    delete[] qdr1Centers;
    delete[] qdr10Centers;
    delete[] pxlData;
    delete[] GCPsW;

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

unsigned int *msumr::MSUProj::getProgressMaxPtr()
{
    return &mProgressMax;
}

unsigned int *msumr::MSUProj::getProgressValPtr()
{
    return &mProgressVal;
}

msumr::RETURN_CODE msumr::MSUProj::calculateBorder(msumr::MSUProj::BORDER_SIDE side, const GCP *gcps,
                                                   const double *geoTransform,
                                                   double *border, const unsigned int &size) const
{
    if (gcps == 0 || border == 0 || geoTransform == 0)
        return ERROR_ARG;

    int gcpStart, gcpInc, gcpEnd;

    if (gcps[0].lat > gcps[mGCPSize -  mGCPXSize + 1].lat)
        switch (side)
        {
        case BORDER_TOP:
            gcpStart = 0;
            gcpInc   = 1;
            gcpEnd   = mGCPXSize - 1;
            break;
        case BORDER_BUTTON:
            gcpStart = mGCPSize - mGCPXSize;
            gcpInc   = 1;
            gcpEnd   = mGCPSize - 1;
            break;
        default:
            break;
        }
    else
        switch (side)
        {
        case BORDER_TOP:
            gcpStart = mGCPSize - 1;
            gcpInc   = -1;
            gcpEnd   = mGCPSize - mGCPXSize;
            break;
        case BORDER_BUTTON:
            gcpStart = mGCPXSize - 1;
            gcpInc   = -1;
            gcpEnd   = 0;
            break;
        default:
            break;
        }

    if (gcps[0].lon < gcps[mGCPXSize - 1].lat)
        switch (side)
        {
        case BORDER_RIGHT:
            gcpStart = mGCPXSize - 1;
            gcpInc   = mGCPXSize;
            gcpEnd   = mGCPSize - 1;
            break;
        case BORDER_LEFT:
            gcpStart = 0;
            gcpInc   = mGCPXSize;
            gcpEnd   = mGCPSize - mGCPXSize;
            break;
        default:
            break;
        }
    else
        switch (side)
        {
        case BORDER_RIGHT:
            gcpStart = mGCPSize - mGCPXSize;
            gcpInc   = -(int)mGCPXSize;
            gcpEnd   = 0;
            break;
        case BORDER_LEFT:
            gcpStart = mGCPSize - 1;
            gcpInc   = -(int)mGCPXSize;
            gcpEnd   = mGCPXSize - 1;
            break;
        default:
            break;
        }

    unsigned int gcpInd = gcpStart + gcpInc;

    double a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
               (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
    double b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;

#ifndef NDEBUG
    std::ofstream bordersCsv(mDstFile + "_border_" + std::to_string(side) + ".csv");
    bordersCsv << "y,x\n";
#endif

    if (side < BORDER_RIGHT)
    {
        double coord    = geoTransform[0];
        double coordInc = geoTransform[1];
        for (unsigned int i = 0; i < size; ++i, coord += coordInc)
        {
            if (gcpInd + gcpInc != gcpEnd)
            {
                if (coord > gcps[gcpInd].lon)
                {
                    gcpInd += gcpInc;
                    a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                        (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
                    b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
                }
            }
            border[i] = coord * a + b;
#ifndef NDEBUG
                bordersCsv << border[i] << "," << coord << "\n";
#endif
        }
    }
    else
    {
        double coord    = geoTransform[3];
        double coordInc = geoTransform[5];
        for (unsigned int i = 0; i < size; ++i, coord += coordInc)
        {
            if (gcpInd + gcpInc != gcpEnd)
            {
                if (coord < gcps[gcpInd].lat)
                {
                    gcpInd += gcpInc;
                    a = (gcps[gcpInd].lat - gcps[gcpInd - gcpInc].lat) /
                        (gcps[gcpInd].lon - gcps[gcpInd - gcpInc].lon);
                    b = gcps[gcpInd - gcpInc].lat - gcps[gcpInd - gcpInc].lon * a;
                }
            }
            border[i] = (coord - b) / a;
#ifndef NDEBUG
                bordersCsv << coord << "," << border[i] << "\n";
#endif
        }
    }

#ifndef NDEBUG
    bordersCsv.close();
#endif

    return SUCCESS;
}

msumr::qdrNode *msumr::MSUProj::sqSurface(const GCP *gcps, unsigned int squareSize,
                                         unsigned int *xSize, unsigned int *ySize) const
{
    if(gcps == 0)
        return 0;
    if(squareSize < 2)
        return 0;

#ifndef NDEBUG
    std::ofstream sqFile(mDstFile + "_" + std::to_string(squareSize) + "_centers.csv");
    sqFile << "y,x\n";
#endif

    --squareSize;
    if (squareSize > 1)
    {
        *xSize = (int)(mGCPXSize / squareSize) + 1;
        *ySize = (int)(mGCPYSize / squareSize) + 1;
    }
    else
    {
        *xSize = mGCPXSize - 1;
        *ySize = mGCPYSize - 1;
    }

    qdrNode *sqCenters = new qdrNode[*xSize * *ySize];
    unsigned int sqIter = 0;
    for (unsigned int sqRow = 0; sqRow < *ySize; ++sqRow)
    {
        unsigned int stepNextRow = squareSize;
        if ((sqRow + 1) * squareSize >= mGCPYSize)
            stepNextRow = mGCPYSize - sqRow * squareSize - 1;
        unsigned int gcpIter = sqRow * squareSize * mGCPXSize;
        for (unsigned int sqCol = 0; sqCol < *xSize; ++sqCol, ++sqIter, gcpIter += squareSize)
        {
            unsigned int stepNextCol = squareSize;
            if ((sqCol + 1) * squareSize >= mGCPXSize)
                stepNextCol = mGCPXSize - sqCol * squareSize - 1;
            sqCenters[sqIter].GCP0 = gcpIter;
            sqCenters[sqIter].lat = (gcps[gcpIter].lat + gcps[gcpIter + stepNextCol].lat +
                gcps[gcpIter + mGCPXSize * stepNextRow].lat + gcps[gcpIter + mGCPXSize * stepNextRow + stepNextCol].lat) / 4;
            sqCenters[sqIter].lon = (gcps[gcpIter].lon + gcps[gcpIter + stepNextCol].lon +
                gcps[gcpIter + mGCPXSize * stepNextRow].lon + gcps[gcpIter + mGCPXSize * stepNextRow + stepNextCol].lon) / 4;
#ifndef NDEBUG
            sqFile << sqCenters[sqIter].lat << "," << sqCenters[sqIter].lon << "\n";
#endif
        }
    }
#ifndef NDEBUG
    sqFile.close();
#endif
    return sqCenters;
}

msumr::TriNode *msumr::MSUProj::triSurface(const msumr::GCP *gcps, unsigned int *xSize, unsigned int *ySize) const
{
    if (gcps == 0)
        return 0;

    *xSize = mGCPXSize - 1;
    *ySize = mGCPYSize - 1;
    TriNode *nodes = new TriNode[*xSize * *ySize];
    unsigned int triInd = 0, gcpInd = 0;
    double x10, x20, y10, y20, z10R, z20R, z10L, z20L;
    for (unsigned int triRow = 0; triRow < *ySize; ++triRow)
    {
        for (unsigned int triCol = 0; triCol < *xSize; ++triCol, ++triInd, ++gcpInd)
        {
            x10  = gcps[gcpInd + 1].lon - gcps[gcpInd].lon;
            x20  = gcps[gcpInd + mGCPXSize].lon - gcps[gcpInd].lon;
            y10  = gcps[gcpInd + 1].lat - gcps[gcpInd].lat;
            y20  = gcps[gcpInd + mGCPXSize].lat - gcps[gcpInd].lat;
            z10R = gcps[gcpInd + 1].x - gcps[gcpInd].x;
            z20R = gcps[gcpInd + mGCPXSize].x - gcps[gcpInd].x;
            z10L = gcps[gcpInd + 1].y - gcps[gcpInd].y;
            z20L = gcps[gcpInd + mGCPXSize].y - gcps[gcpInd].y;
            nodes[triInd].col0 = gcps[gcpInd].x;
            nodes[triInd].row0 = gcps[gcpInd].y;
            nodes[triInd].aCol = y10 * z20R - z10R * y20;
            nodes[triInd].bCol = x10 * z20R - x20 * z10R;
            nodes[triInd].aRow = y10 * z20L - z10L * y20;
            nodes[triInd].bRow = x10 * z20L - x20 * z10L;
            nodes[triInd].c  = x10 * y20 - x20 * y10;
            nodes[triInd].lon0 = gcps[gcpInd].lon;
            nodes[triInd].lat0 = gcps[gcpInd].lat;
        }
        ++gcpInd;
    }
    return nodes;
}
