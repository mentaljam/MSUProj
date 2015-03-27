#include "msugeo.h"

using namespace std;
using namespace msumr;

int main(int argc, char *argv[])
{
    GDALAllRegister();
    msugeo msuObj;

    char *srcFile;
    char *gcpFile;
    string dstFile;
    for (short i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 ||
                strcmp(argv[i], "--help") == 0)
            {
                printf("\nmsugeo-cli - a command line utility\n"
                       "             for projecting images of MSU-MR equipment\n"
                       "             of Russian ERS sattelite Meteor-M\n\n"
                       "Author:  Petr Tsymbarovich <tpr@ntsomz.ru>\n"
                       "         Research Center for Earth Operative Monitoring\n"
                       "         (NTs OMZ) <www.ntsomz.ru>\n\n"
                       "Version: %s\n\n"
                       "Usage:   msugeo-cli [OPTIONS] -src input_file -gcp gcp_file [-gcp output_file]\n\n"
                       "Options:\n"
                       "   -u        - Produce an image in UTM projection\n"
                       "               (a zone number is calculated with center point)\n"
                       "   -f [name] - Output raster format (it must support GDAL create method\n", msuObj.getVersion());
                return 0;
            }
            else if (i + 1 < argc && strcmp(argv[i], "-src") == 0)
                srcFile = argv[++i];
            else if (i + 1 < argc && strcmp(argv[i], "-gcp") == 0)
                gcpFile = argv[++i];
            else if (i + 1 < argc && strcmp(argv[i], "-dst") == 0)
                dstFile = argv[++i];
            else if (strcmp(argv[i], "-u") == 0)
                msuObj.useUTM(true);
            else if (i + 1 < argc && strcmp(argv[i], "-f") == 0)
                msuObj.setDSTFormat(argv[++i]);
            else
                printf("WARNING: Unknown option \"%s\"\n", argv[i]);argv[i];
        }
    }

    if (srcFile == NULL || gcpFile == NULL)
    {
        printf("ERROR: input and output not specified\n"
               "print \"-h\" for help\n");
        return 1;
    }

    if (dstFile.empty())
        dstFile = srcFile + string(".tif");

    retCode code;

    code = msuObj.setSRC(srcFile);
    if (code != success)
    {
        printf("ERROR: can not read input file\n");
        return code;
    }

    code = msuObj.readGCP(gcpFile);
    if (code != success)
    {
        printf("ERROR: can not read gcp file\n");
        return code;
    }

    msuObj.setDST(dstFile.c_str());

    printf("Processing warp operation\n");
    code = msuObj.warp();
    switch (code) {
    case errSRC:
        printf("ERROR: can not read input file\n");
        break;
    case errDST:
        printf("ERROR: can not write output file\n");
        break;
    case errGCP:
        printf("ERROR: can not read gcp file\n");
        break;
    default:
        printf("Finished successfully\n");
        break;
    }

    return 0;
}

