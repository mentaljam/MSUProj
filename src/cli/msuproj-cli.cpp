#include "msuproj.h"
#include "iostream"

using namespace std;
using namespace MSUMR;

int main(int argc, char *argv[])
{
    GDALAllRegister();

    bool useUTM = false;
    bool zerosAsND = false;

    MSUProj msuObj;

    string srcFile;
    string gcpFile;
    string dstFile;

    for (unsigned short i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
            {
                cout << "\nmsuproj-cli - a command line utility for projecting images\n"
                        "              of MSU-MR equipment of Russian ERS sattelite Meteor-M\n\n"
                        "Author:        Petr Tsymbarovich <tpr@ntsomz.ru>\n"
                        "               Research Center for Earth Operative Monitoring\n"
                        "               (NTs OMZ) <www.ntsomz.ru>\n\n"
                        "Project Page:  https://github.com/mentaljam/MSUProj\n\n"
                        "Version:       " << msuObj.getVersion() << " (" << msuObj.getVersion(1) << ") for " << msuObj.getVersion(2) << "\n\n"
                        "Usage:   msuproj-cli [OPTIONS] -src input_file -gcp gcp_file [-dst output_file]\n\n"
                        "Options:\n"
                        "    -u              Produce an image in UTM projection\n"
                        "                    (a zone number is calculated for the center point)\n"
                        "    -z              Set NoData value to zero\n"
                        "    -v | --version  Print version number and exit\n"
                        "    -h | --help     Print help message and exit" << endl;
                return 0;
            }
            if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
            {
                cout << msuObj.getVersion() << endl;
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
                clog << "WARNING: Unknown option '" << argv[i] << "'" << endl;
        }
    }

    if (srcFile.empty() || gcpFile.empty())
    {
        cerr << "ERROR: input files are not specified\n"
                "print '-h' for help" << endl;
        return 1;
    }

    retCode code;

    code = msuObj.setSRC(srcFile);
    if (code != success)
    {
        cerr << "ERROR: can not read input file" << endl;
        return code;
    }

    code = msuObj.readGCP(gcpFile);
    if (code != success)
    {
        cerr << "ERROR: can not read gcp file" << endl;
        return code;
    }

    string UTMZone =  msuObj.getUTM();

    if (dstFile.empty())
    {
        auto extCut = srcFile.find_last_of('.');
        dstFile = srcFile.substr(0, extCut) + "_proj";
        if (useUTM)
            dstFile += "_" + UTMZone;
        dstFile += ".tif";
    }

    msuObj.setDST(dstFile);

    cout << "Processing warp operation";
    if (useUTM)
        cout << " in UTM mode using zone " << UTMZone;
    else
        cout << " in LatLon mode";
    if (zerosAsND)
        cout << " and NoData=0";
    cout << endl;
    clock_t sTime = clock();
    code = msuObj.warp(useUTM, zerosAsND);
    int exeTime = (clock() - sTime) / CLOCKS_PER_SEC;
    switch (code) {
    case errSRC:
        cerr << "ERROR: can not read input file" << endl;
        break;
    case errDST:
        cerr << "ERROR: can not write output file" << endl;
        break;
    case errGCP:
        cerr << "ERROR: can not read gcp file" << endl;
        break;
    default:
        cout << "Finished successfully in " << (int)(exeTime / 60)
             << " min " << exeTime - ((int)(exeTime / 60)) * 60 << " sec\n"
                "Output raster: " << dstFile << endl;
        break;
    }

    return 0;
}

