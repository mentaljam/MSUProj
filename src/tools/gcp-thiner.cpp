#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: gcp-thiner input.gcp line_count [output.gcp]");
        return 1;
    }

    int lCount = atoi(argv[2]);
    if (lCount <= 1)
    {
        printf("ERROR: wrong line conter %s, must be greater than 1", argv[2]);
        return 1;
    }

    ifstream srcGcp(argv[1]);
    if (srcGcp.fail())
    {
        printf("ERROR: error reading input gcp file");
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
        printf("ERROR: error writing output gcp file");
        return 4;
    }

    string line;
    for (int i = lCount; getline(srcGcp, line); ++i)
        if (i == lCount)
        {
            i = 0;
            dstGcp << line << endl;
        }

    srcGcp.close();
    dstGcp.close();
    return 0;
}
