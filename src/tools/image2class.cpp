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
    outstream << "#include <cstring>" << endl << endl
              << "/// @brief " << argv[2] << " image class" << endl
              << "class " << argv[2] << endl
              << "{" << endl
              << "public:" << endl
              << "    " << argv[2] << "();" << endl
              << "    ~" << argv[2] << "();"  << endl
              << "    unsigned int  width;" << "  ///< Number of rows in image" << endl
              << "    unsigned int  height;" << " ///< Number of lines in image" << endl
              << "    unsigned int  bands;" << "  ///< Number of image bands" << endl
              << "    unsigned int  size;" << "   ///< Number of pixels in image" << endl
              << "    unsigned char *data;" << "  ///< Image data array" << endl
              << "};" << endl << endl
              << argv[2] << "::" << argv[2] << "()" << endl
              << "{" << endl
              << "    width  = " << x << ";" << endl
              << "    height = " << y << ";" << endl
              << "    bands  = " << bands << ";" << endl
              << "    size   = " << x * y * bands + 1 << ";" << endl << endl
              << "    data = new unsigned char[size];" << endl
              << "    memcpy(data," << endl;

    int i, j;
    unsigned char *tmpData;

    for (int b = 0; b < bands; ++b)
    {
        tmpData = new unsigned char[size]();
        tt->GetRasterBand(b + 1)->RasterIO(GF_Read, 0, 0, x, y, tmpData,
                                           x, y, GDT_Byte, 0, 0);
        for (i = 0; i < y; ++i)
        {
            outstream << "           \"";
            for (j = 0; j < x; ++j)
                outstream << "\\x" << std::hex << (int)tmpData[i * x + j];
            outstream << "\"" << endl;
        }
        delete[] tmpData;
    }

    outstream << "           \"\\0\"," << endl
              << "           size);" << endl
              << "}" << endl << endl
              << argv[2] << "::~" << argv[2] << "()" << endl
              << "{" << endl
              << "    if (data)" << endl
              << "        delete[] data;" << endl
              << "}" << endl;

    outstream.close();

    return 0;
}