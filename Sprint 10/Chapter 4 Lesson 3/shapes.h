#pragma once
#include "texture.h"

#include <memory>

// �������������� ���� �����: ������������� � ������
enum class ShapeType { RECTANGLE, ELLIPSE };

class Shape {
public:
    // ������ ����� �������� ����� ������� ���������� � ������,
    // � ����� �� ����� ��������
    explicit Shape(ShapeType type)
        : type_(type)
    {
    }

    void SetPosition(Point pos) {
        pos_ = pos;
    }

    void SetSize(Size size) {
        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = std::move(texture);
    }

    // ������ ������ �� ��������� �����������
    // � ����������� �� ���� ������ ������ ���������� ���� ������, ���� �������������
    // ������� ������, ��������� �� ������� ��������, � ����� � ������, ����� �������� �� ������,
    // ������ ������������ � ������� ������� ����� '.'
    // ����� ������, ��������� �� ������� ������� image, ������ �������������.
    void Draw(Image& image) const {
        Size image_size = GetImageSize(image);
        Size texture_size = texture_ ? texture_->GetSize() : Size{ 0, 0 };
        int x_max = std::min(pos_.x + size_.width - 1, image_size.width - 1);
        int y_max = std::min(pos_.y + size_.height - 1, image_size.height - 1);
                
        for (int y = pos_.y; y <= y_max; ++y) {
            for (int x = pos_.x; x <= x_max; ++x) {
                int texture_x = x - pos_.x;
                int texture_y = y - pos_.y;
                if (type_ == ShapeType::RECTANGLE && IsPointInRectangle({ texture_x, texture_y }, size_)) {
                    if (texture_x < texture_size.width && texture_y < texture_size.height) {
                        image[y][x] = texture_->GetPixelColor({ texture_x, texture_y });
                    }
                    else image[y][x] = '.';
                }
                if (type_ == ShapeType::ELLIPSE && IsPointInEllipse({ texture_x, texture_y }, size_)) {
                    if (texture_x < texture_size.width && texture_y < texture_size.height) {
                        image[y][x] = texture_->GetPixelColor({ texture_x, texture_y });
                    }
                    else image[y][x] = '.';
                }
            }
        }
    }

private:
    ShapeType type_;
    Point pos_;
    Size size_;
    std::shared_ptr<Texture> texture_;
};