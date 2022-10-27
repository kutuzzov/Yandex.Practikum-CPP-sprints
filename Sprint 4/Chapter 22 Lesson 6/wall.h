#pragma once

class Wall {
public:
    enum class Color {
        BLUE,
        GREEN,
        RED,
        WHITE,
        YELLOW
    };

    Wall(double width, double height);

    double GetHeight() const;
    double GetWidth() const;
    void SetColor(Color color);
    Color GetColor() const;

private:
    double width_;
    double height_;
    Color color_;
};
