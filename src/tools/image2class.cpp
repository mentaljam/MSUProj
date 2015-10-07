#include <gdal_priv.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("\nUsage: image2class input_image class_name\n\n");
        return 1;
    }

    GDALAllRegister();

    GDALDataset *tt = (GDALDataset*)GDALOpen(argv[1], GA_ReadOnly);
    int x = tt->GetRasterXSize();
    int y = tt->GetRasterYSize();
    int size = x * y;
    int bands = tt->GetRasterCount();

    std::ofstream outstream;

    string out = string(argv[1]) + ".hpp";

    outstream.open(out.c_str());
    outstream << "#include <cstring>\n\n"
                 "/// @brief " << argv[2] << " image class\n"
                 "class " << argv[2] << "\n"
                 "{\n"
                 "public:\n"
                 "    " << argv[2] << "();\n"
                 "    ~" << argv[2] << "();\n"
                 "    unsigned int  width;" << "  ///< Number of columns in image\n"
                 "    unsigned int  height;" << " ///< Number of lines in image\n"
                 "    unsigned int  bands;" << "  ///< Number of image bands\n"
                 "    unsigned int  size;" << "   ///< Number of pixels in band\n"
                 "    unsigned char **data;" << " ///< Image data array\n"
                 "};\n\n"
              << argv[2] << "::" << argv[2] << "() :\n"
                 "    width(" << x << "),\n"
                 "    height(" << y << "),\n"
                 "    bands(" << bands << "),\n"
                 "    size (" << x * y << "),\n"
                 "    data(new unsigned char*[" << bands << "])\n"
                 "{\n";

    int i, j;
    unsigned char *tmpData;
    tmpData = new unsigned char[size]();

    for (int b = 0; b < bands; ++b)
    {
        outstream << "    data[" << b << "] = new unsigned char[size];\n";
        tt->GetRasterBand(b + 1)->RasterIO(GF_Read, 0, 0, x, y, tmpData,
                                           x, y, GDT_Byte, 0, 0);
        for (i = 0; i < y; ++i)
        {
            outstream << "    memcpy(&data[" << b << "][" << i * x << "],\n"
                         "           \"" << std::hex;
            for (j = 0; j < x; ++j)
            {
                int pixVal = (int)tmpData[i * x + j];
                if (pixVal < 3)
                    pixVal = 3;
                outstream << "\\x" << pixVal;
            }
            outstream << "\",\n"
                         "           width);\n" << std::dec;
        }
        memset(tmpData, 0, size);
    }
    delete[] tmpData;

    outstream << "}\n\n"
              << argv[2] << "::~" << argv[2] << "()\n"
                 "{\n"
                 "    for (int b = 0; b < bands; ++b)\n"
                 "        if (data[b])\n"
                 "            delete[] data[b];\n"
                 "    if (data)\n"
                 "        delete[] data;\n"
                 "}\n";

    outstream.close();

    return 0;
}
