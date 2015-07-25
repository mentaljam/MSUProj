#ifndef MSUPROJ_H
#define MSUPROJ_H

#include <gdal_priv.h>
#ifdef _WIN32
#   include <msuproj/msuproj_export.h>
#else
#   define MSUPROJ_EXPORT
#endif

namespace MSUMR {

/**
 * @brief Enumeration of exit codes
 */
enum retCode
{
    success, ///< Successful exit
    errSRC,  ///< Error of reading of source raster
    errDST,  ///< Error of creation of destination raster
    errGCP   ///< Error of reading of GCP file
};

/**
 * @brief Ground control point (GCP) structure
 */
struct gcp
{
    int x;      ///< The row of the source data matrix
    int y;      ///< The line of the source data matrix
    double lon; ///< Latitude
    double lat; ///< Longitude
};

/**
 * @brief The class for reprojecting geo data
 * of sensing equipment MSU-MR of ERS satellite Meteor-M
 */
class MSUPROJ_EXPORT MSUProj
{
public:

    /**
     * @brief Constructor
     */
    MSUProj();

    /**
     * @brief Destructor
     */
    ~MSUProj();

    /**
     * @brief Method of getting version of MSUProj
     * @param type - type of returning value:
     * - 0 - Get version number
     * - 1 - Get build date
     * - 2 - Get build architecture
     * @return Version value
     */
    const char* getVersion(const unsigned int &type = 0) const;

    /**
     * @brief Method of setting destination file name
     * @param file Destination file name
     */
    void setDST(std::string file);

    /**
     * @brief Method of setting destination file format
     * @param format Destination file format
     */
    void setDSTFormat(std::string format);

    /**
     * @brief Method of setting source raster file
     * @param file Source file name
     * @return Exit code MSUMR::retCode:
     * - success - Successful reading of source file
     * - errSRC - Error of reading of source file
     */
    const retCode setSRC(std::string file);

    /**
     * @brief Method of setting GCP file
     * @param file GCP file name
     * @return Exit code MSUMR::retCode:
     * - success - Successful reading of GCP file
     * - errGCP - Error of reading of GCP file
     */
    const retCode readGCP(std::string file);

    /**
     * @brief Method of setting maximum size of perimeter of interpolation of pixel values
     * @param perim Perimeter size in pixels
     */
    void setPerimSize(const unsigned int &perim);

    const std::string getUTM() const;

    /**
     * @brief Method of projecting of MSU-MR frame
     * @return Exit code MSUMR::retCode:
     * - success - Successful projection
     * - errDST - Error of creation of destination raster
     * - errSRC - Не указан исходный растр
     * - errGCP - Не указан файл GCP
     */
    const retCode warp(const bool &useUTM = false, const bool &zerosAsND = false);

    unsigned int getSrcXSize() const;

    unsigned int getSrcYSize() const;

    unsigned int getGcpXSize() const;

    unsigned int getGcpYSize() const;

    unsigned int getGcpXStep() const;

    unsigned int getGcpYStep() const;

private:

    /**
     * @ru
     * @brief Перечисление для вычисления
     * угловых координат выходного растра
     */
    enum compareCoords
    {
        minLON, ///< @ru Минимальная долгота
        maxLON, ///< @ru Максимальная долгота
        minLAT, ///< @ru Минимальная широта
        maxLAT  ///< @ru Максимальная широта
    };

    GDALDataset* srcDS; ///< @ru Исходный растр
    GDALDataset* dstDS; ///< @ru Выходной растр

    gcp* gcps;          ///< @ru Массив точек геосетки

    std::string dstFile;   ///< @ru Файл назначения
    std::string dstFormat; ///< @ru Формат выходного файла (должен поддерживать GDALCreate)

    bool hemisphere; ///< @ru Полушарие:
                     /// - 0 - южное
                     /// - 1 - северное
    unsigned int zone;        ///< @ru Номер зоны UTM

    unsigned int perimSize;   ///< @ru Максимальный периметр поиска пикселей для интерполяции

    unsigned int srcXSize; ///< @ru Количество столбцов исходного растра
    unsigned int srcYSize; ///< @ru Количество строк исходного растра
    unsigned int srcSize;  ///< @ru Количество пикселей исходного растра

    unsigned int gcpXSize; ///< @ru Количество столбцов геосетки
    unsigned int gcpYSize; ///< @ru Количество строк геосетки
    unsigned int gcpXStep; ///< @ru Шаг столбцов геосетки относительно исходного растра
    unsigned int gcpYStep; ///< @ru Шаг строк геосетки относительно исходного растра
    unsigned int gcpSize;  ///< @ru Количество точек геосетки

    /**
     * @ru Указатель на массив 6 коэффициентов
     * аффиного трансформирования выходного растра
     * (аналог GDAL padfTransform), где
     * - geoTransform[0] - x координата верхнего левого угла,
     * - geoTransform[1] - разрешение пиксела по оси x,
     * - geoTransform[2] - поворот (0 для ориентации на север),
     * - geoTransform[3] - y координата верхнего левого угла,
     * - geoTransform[4] - поворот (0 для ориентации на север),
     * - geoTransform[5] - разрешение пиксела по оси y.
     */
    double *geoTransform;

    /**
     * @ru
     * @brief Метод замены десятичного разделителя:
     * запятые (,) на точки (.)
     * @param str Исходная строка
     * @return Преобразованная строка
     */
    std::string comma2dot(std::string str) const;
};

}

#endif // MSUPROJ_H
