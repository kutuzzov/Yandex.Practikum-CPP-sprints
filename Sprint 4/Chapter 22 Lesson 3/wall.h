class Wall {
public:
    Wall(double width, double height)
    : width_(width)
    , height_(height) {
    }

    double GetHeight() const {
        return height_;
    }
    double GetWidth() const {
        return width_;
    }
private:
    double width_;
    double height_;
};
