#include <gdal_priv.h>
#include <iostream>
#include <algorithm>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "\nUsage: image2class input_image class_name\n\n";
        return 1;
    }

    GDALAllRegister();

    GDALDataset *tt = (GDALDataset*)GDALOpen(argv[1], GA_ReadOnly);
    int x = tt->GetRasterXSize();
    int y = tt->GetRasterYSize();
    size_t size = x * y;
    int bands = tt->GetRasterCount();

    auto srcData = new unsigned char[size]();
    unsigned char *dstData[bands];
    size_t mDeflatedDataSize[bands];
    std::string initializerListString;
    size_t deflatedDataSizeTotal = 0;

    for (int b = 0; b < bands; ++b)
    {
        tt->GetRasterBand(b + 1)->RasterIO(GF_Read, 0, 0, x, y, srcData, x, y, GDT_Byte, 0, 0);
        for (int i = 0; i < size; ++i)
        {
            if ((int)srcData[i] < 3)
            {
                srcData[i] = 0x3;
            }
        }

        dstData[b] = (unsigned char *)CPLZLibDeflate(srcData, size, -1, 0, 0, &mDeflatedDataSize[b]);

        deflatedDataSizeTotal += mDeflatedDataSize[b];
        initializerListString += std::to_string(mDeflatedDataSize[b]);
        if (b != bands - 1)
        {
            initializerListString += ", ";
        }

        memset(srcData, 0, size);
    }

    delete[] srcData;

    std::string hppName(argv[2]);
    std::transform(hppName.begin(), hppName.end(), hppName.begin(), ::toupper);
    hppName += "_HPP";

    std::cout << "#ifndef " << hppName << "\n"
                 "#define " << hppName << "\n\n"
                 "#include <cstring>\n"
                 "#include <gdal_priv.h>\n\n"
                 "/// @brief " << argv[2] << " image class\n"
                 "class " << argv[2] << "\n"
                 "{\n"
                 "public:\n"
                 "    " << argv[2] << "() {}\n"
                 "    unsigned int width() { return " << x << "; } ///< Number of columns in image\n"
                 "    unsigned int height() { return " << y << "; } ///< Number of lines in image\n"
                 "    unsigned int bands() { return " << bands << "; } ///< Number of image bands\n"
                 "    unsigned int size() { return " << x * y << "; } ///< Number of pixels in band\n"
                 "    /// Write band data to buffer\n"
                 "    unsigned char *data(const unsigned int &band)\n"
                 "    {\n"
                 "        if (band >= this->bands())\n"
                 "            return 0;\n"
                 "        size_t offset = 0;\n"
                 "        for (unsigned int b = 0; b < band; offset += mDeflatedDataSize[b], ++b);\n"
                 "        return (unsigned char *)CPLZLibInflate(&mData[offset], mDeflatedDataSize[band], 0, 0, 0);\n"
                 "    }\n\n"
                 "private:\n"
                 "    static const unsigned char mData[" << deflatedDataSizeTotal << "]; ///< Image data array\n"
                 "    unsigned int mDeflatedDataSize[" << bands << "] = {" << initializerListString << "}; ///< Sizes of deflated data of bands\n"
                 "};\n\n"
                 "const unsigned char " << argv[2] << "::mData[] = {\n    "
              << std::hex;

    for (int b = 0; b < bands; ++b)
    {
        int width = 0;
        for (size_t i = 0; i < mDeflatedDataSize[b]; ++i, ++width)
        {
            if (width > 15)
            {
                std::cout << "\n    ";
                width = 0;
            }
            std::cout << "0x" << (int)dstData[b][i] << ",";
        }
        VSIFree(dstData[b]);
    }

    std::cout << std::dec
              << "\n};\n\n"
                 "#endif // " << hppName << "\n";

    return 0;
}
