#include "wall.h"

Wall::Wall(double width, double height) 
    : width_(width), height_(height), color_(Color::WHITE) 
{
}

double Wall::GetHeight() const {
    return height_;
}
double Wall::GetWidth() const {
    return width_;
}
void Wall::SetColor(Color color) {
    color_ = color;
}
Wall::Color Wall::GetColor() const {
	return color_;
}
