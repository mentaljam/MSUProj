#include "msugeo.h"

using namespace std;
using namespace msumr;

int main(int argc, char *argv[])
{
    GDALAllRegister();

    bool useUTM = false;
    bool zerosAsND = false;

    msugeo msuObj;

    string srcFile;
    string gcpFile;
    string dstFile;

    for (short i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            {
                printf("\nmsugeo-cli - a command line utility for projecting images\n"
                       "             of MSU-MR equipment of Russian ERS sattelite Meteor-M\n\n"
                       "Author:  Petr Tsymbarovich <tpr@ntsomz.ru>\n"
                       "         Research Center for Earth Operative Monitoring\n"
                       "         (NTs OMZ) <www.ntsomz.ru>\n\n"
                       "Version: %s (%s) for %s\n\n"
                       "Usage:   msugeo-cli [OPTIONS] -src input_file -gcp gcp_file [-dst output_file]\n\n"
                       "Options:\n"
                       "    -u              Produce an image in UTM projection\n"
                       "                    (a zone number is calculated for center point)\n"
                       "    -z              Set NoData value to zero\n"
                       "    -v | --version  Print version number and exit\n"
                       "    -h | --help     Print help message and exit\n",
                       msuObj.getVersion(), msuObj.getVersion(1), msuObj.getVersion(2));
                return 0;
            }
            if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
            {
                printf("%s\n", msuObj.getVersion());
                return 0;
            }
            else if (i + 1 < argc && !strcmp(argv[i], "-src"))
                srcFile = argv[++i];
            else if (i + 1 < argc && !strcmp(argv[i], "-gcp"))
                gcpFile = argv[++i];
            else if (i + 1 < argc && !strcmp(argv[i], "-dst"))
                dstFile = argv[++i];
            else if (!strcmp(argv[i], "-u"))
                useUTM = true;
            else if (!strcmp(argv[i], "-z"))
                zerosAsND = true;
            else if (i + 1 < argc && !strcmp(argv[i], "-f"))
                msuObj.setDSTFormat(argv[++i]);
            else
                printf("WARNING: Unknown option \"%s\"\n", argv[i]);argv[i];
        }
    }

    if (srcFile.empty() || gcpFile.empty())
    {
        printf("ERROR: input and output not specified\n"
               "print \"-h\" for help\n");
        return 1;
    }

    if (dstFile.empty())
        dstFile = srcFile + string(".tif");

    retCode code;

    code = msuObj.setSRC(srcFile.c_str());
    if (code != success)
    {
        printf("ERROR: can not read input file\n");
        return code;
    }

    code = msuObj.readGCP(gcpFile.c_str());
    if (code != success)
    {
        printf("ERROR: can not read gcp file\n");
        return code;
    }

    msuObj.setDST(dstFile.c_str());

    printf("Processing warp operation");
    if (useUTM)
        printf(" in UTM mode using zone %s", msuObj.getUTM());
    else
        printf(" in LatLon mode");
    if (zerosAsND)
        printf(" and NoData=0");
    printf("\n");
    code = msuObj.warp(useUTM, zerosAsND);
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
        printf("Finished successfully\n"
               "Output raster: %s\n", dstFile.c_str());
        break;
    }

    return 0;
}

