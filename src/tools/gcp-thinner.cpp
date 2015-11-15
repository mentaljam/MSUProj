#include <fstream>
#include <msucore.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: gcp-thiner input.gcp step [output.gcp]\n");
        return 1;
    }

    int step = atoi(argv[2]);
    if (step <= 1)
    {
        printf("ERROR: wrong step %s, must be greater than 1\n", argv[2]);
        return 1;
    }

    ifstream srcGcp(argv[1]);
    if (srcGcp.fail())
    {
        printf("ERROR: error reading input gcp file\n");
        return 3;
    }

    string dstName;
    if (argc > 3)
        dstName = argv[3];
    else
        dstName = string(argv[1]) + "_" + string(argv[2]);
    ofstream dstGcp(dstName.c_str());
    if (dstGcp.fail())
    {
        printf("ERROR: error writing output gcp file\n");
        return 4;
    }

    std::string line;

    unsigned int gcpSize = 0;
    while(getline(srcGcp, line))
        ++gcpSize;
    srcGcp.clear();
    srcGcp.seekg(0);

    msumr::GCP *gcps = new msumr::GCP[gcpSize];
    unsigned int gcpInd = 0;
    size_t pos;
    while(getline(srcGcp, line))
    {
        line = msumr::comma2dot(line);

        gcps[gcpInd].x = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        gcps[gcpInd].y = std::stoi(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        gcps[gcpInd].lat = std::stod(line, &pos);
        line = line.substr(++pos, line.size() - pos);

        gcps[gcpInd].lon = std::stod(line, &pos);

        ++gcpInd;
    }

    unsigned int  gcpXSize = 1;
    while (gcps[gcpXSize].x != 0)
        ++gcpXSize;
    unsigned int gcpYSize = gcpSize / gcpXSize;

    for (unsigned int gRow = 0; gRow < gcpYSize; gRow += step)
        for (unsigned int gCol = 0; gCol < gcpXSize; gCol += step)
        {
            unsigned int i = gRow * gcpXSize + gCol;
            dstGcp << gcps[i].x << " " << gcps[i].y << " "
                   << gcps[i].lat << " " << gcps[i].lon << "\n";
        }

    delete[] gcps;
    srcGcp.close();
    dstGcp.close();

    return 0;
}
