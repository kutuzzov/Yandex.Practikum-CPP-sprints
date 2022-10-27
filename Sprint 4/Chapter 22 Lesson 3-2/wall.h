class Wall {
public:
    enum class Color { BLUE, GREEN, RED, WHITE, YELLOW };

    Wall(double width, double height)
        : width_(width)
        , height_(height)
        , color_(Color::WHITE) {
    }

    double GetHeight() const {
        return height_;
    }
    double GetWidth() const {
        return width_;
    }
    void SetColor(Color color) {
        color_ = color;
    }
    Color GetColor() const {
        return color_;
    }

private:
    double width_;
    double height_;
    Color color_;
};
