#pragma once

#include <string>
#include <vector>

struct Size {
    int width = 0;
    int height = 0;
};

struct Point {
    int x = 0;
    int y = 0;
};

// Изображение. Пиксели это символы.
// Первый индекс (по std::vector) - строки изображения, координата y
// Второй индекс (по std::string) - столбцы изображения, координата x
// Предполагается, что длина всех строк одинакова
using Image = std::vector<std::string>;

inline Size GetImageSize(const Image& image) {
    const int width = image.empty() ? 0 : static_cast<int>(image[0].size());
    const int height = static_cast<int>(image.size());
    return { width, height };
}

// Проверяет, содержится ли заданная точка в эллипсе заданного размера
// Считается, что эллипс вписан в прямоугольник с вершинами в точках (0, 0) и
// (size.width, size.height)
inline bool IsPointInEllipse(Point p, Size size) {
    // Нормируем координаты точки в диапазон (-1, 1)
    // Сдвиг на 0.5 нужен, чтобы считать расстояние до центра пикселя, так
    // получается более красивая форма
    double x = (p.x + 0.5) / (size.width / 2.0) - 1;
    double y = (p.y + 0.5) / (size.height / 2.0) - 1;
    // Проверяем, лежит ли точка в единичном круге
    return x * x + y * y <= 1;
}

inline bool IsPointInRectangle(Point p, Size size) {
    return (p.x <= size.width && p.y <= size.height);
}