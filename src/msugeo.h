#ifndef MSUGEO_H
#define MSUGEO_H

#include <gdal_priv.h>

namespace msumr {

/**
 * @ru
 * @brief Перечисление кодов завершения функций
 */
enum retCode
{
    success, ///< @ru Успешное завершение
    errSRC,  ///< @ru Ошибка чтения исходного растра
    errDST,  ///< @ru Ошибка создания растра назначения
    errGCP   ///< @ru Ошибка чтения файла GCP
};

/**
 * @ru
 * @brief Структура GCP
 */
struct gcp
{
    int x;      ///< @ru столбец исходной матрицы
    int y;      ///< @ru строка исходной матрицы
    double lon; ///< @ru Долгота
    double lat; ///< @ru Широта
};

/**
 * @ru
 * @brief Класс проецирования данных
 * аппаратуры МСУ-МР КА Метеор-М
 */
class msugeo
{
public:

    /**
     * @ru
     * @brief Стандартный конструктор
     */
    msugeo();

    /**
     * @ru
     * @brief Стандартный деструктор
     */
    ~msugeo();

    /**
     * @ru
     * @brief Метод получения версии проекта
     * @return Номер версии
     */
    const char* getVersion();

    /**
     * @ru
     * @brief Метод указания имени файла назначения
     * @param file Имя файла назначения
     */
    void setDST(const char *file);

    void setDSTFormat(const char *format);

    /**
     * @ru
     * @brief Метод указания исходного растра
     * @param file Имя исходного файла
     * @return Код завершения функции msumr::retCode:
     * - success - Успешное чтение исходного растра
     * - errSRC - Ошибка чтения исходного растра
     */
    retCode setSRC(const char *file);

    /**
     * @ru
     * @brief Метод указания файла GCP
     * @param file Имя файла GCP
     * @return Код завершения функции msumr::retCode:
     * - success - Успешное чтение файла GCP
     * - errGCP - Ошибка чтения файла GCP
     */
    retCode readGCP(const char *file);

    /**
     * @ru
     * @brief Метод указания максимального периметра поиска
     * пикселей для интерполяции
     * @param perim Значение периметра
     */
    void setPerimSize(int perim);

    /**
     * @ru
     * @brief Строить или нет файл в проекции UTM
     */
    void useUTM(bool);

    /**
     * @ru
     * @brief Метод проецирования кадра
     * @return Код завершения функции msumr::retCode:
     * - success - Успешное завершение проецирования
     * - errDST - Ошибка создания растра назначения
     * - errSRC - Не указан исходный растр
     * - errGCP - Не указан файл GCP
     */
    retCode warp();

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

    char *dstFile;   ///< @ru Файл назначения
    char *dstFormat; ///< @ru Формат выходного файла (должен поддерживать GDALCreate)

    gcp* gcps;      ///< @ru Массив точек геосетки

    bool ifUTM;     ///< @ru Переключатель построения построения изображения в проекции UTM

    int zone;       ///< @ru Номер зоны UTM
    int hemisphere; ///< @ru Полушарие:
                    /// - 0 - южное
                    /// - 1 - северное
    int perimSize;  ///< @ru Максимальный периметр поиска пикселей для интерполяции

    int srcXSize; ///< @ru Количество столбцов исходного растра
    int srcYSize; ///< @ru Количество строк исходного растра
    int srcSize;  ///< @ru Количество пикселей исходного растра

    int gcpXSize; ///< @ru Количество столбцов геосетки
    int gcpYSize; ///< @ru Количество строк геосетки
    int gcpXStep; ///< @ru Шаг столбцов геосетки относительно исходного растра
    int gcpYStep; ///< @ru Шаг строк геосетки относительно исходного растра
    int gcpSize;  ///< @ru Количество точек геосетки

    int dstXSize; ///< @ru Количество столбцов выходного растра
    int dstYSize; ///< @ru Количество строк выходного растра
    int dstSize;  ///< @ru Количество пикселей выходного растра

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
    std::string comma2dot(std::string str);
};

}

#endif // MSUGEO_H
