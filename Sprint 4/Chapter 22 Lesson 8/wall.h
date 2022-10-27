#pragma once
#include <utility>

class Wall {
public:
    enum class Color { BLUE, GREEN, RED, WHITE, YELLOW };

    Wall(double width, double height);

    double GetHeight() const;
    double GetWidth() const;
    std::pair<double, double> GetSizes() const;
    void SetColor(Color color);
    Color GetColor() const;
    bool IsDoorInstalled() const;
    void SetDoorInstalled();

private:
    double width_;
    double height_;
    Color color_;
    bool is_door_installed_;
};