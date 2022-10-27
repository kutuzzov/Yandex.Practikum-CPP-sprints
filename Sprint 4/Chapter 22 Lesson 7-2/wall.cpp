#include "wall.h"

Wall::Wall(double width, double height)
    : width_(width)
    , height_(height)
    , color_(Color::WHITE)
    , is_door_installed_(false) {
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
bool Wall::IsDoorInstalled() const {
    return is_door_installed_;
}
void Wall::SetDoorInstalled() {
    is_door_installed_ = true;
}