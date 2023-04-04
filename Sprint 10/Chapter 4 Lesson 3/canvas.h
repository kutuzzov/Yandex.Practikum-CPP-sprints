#include "shapes.h"

#include <iostream>
#include <map>
#include <string_view>

// Холст, на котором размещаются фигуры
class Canvas {
public:
    using ShapeId = size_t;

    explicit Canvas(Size size)
        : size_(size) {
    }

    void SetSize(Size size) {
        size_ = size;
    }

    // Добавляет на холст фигуру указанного типа, 
    ShapeId AddShape(ShapeType shape_type, Point position, Size size,
        std::shared_ptr<Texture> texture) {
        auto shape = std::make_unique<Shape>(shape_type);
        shape->SetPosition(position);
        shape->SetSize(size);
        shape->SetTexture(std::move(texture));
        return InsertShape(std::move(shape));
    }

    // Создаёт копию фигуры в новых координатах
    ShapeId DuplicateShape(ShapeId source_id, Point target_position) {
        auto shape = std::make_unique<Shape>(*GetShapeNodeById(source_id)->second);
        shape->SetPosition(target_position);
        return InsertShape(std::move(shape));
    }

    // Удаляет фигуру с заданным id
    void RemoveShape(ShapeId id) {
        shapes_.erase(GetShapeNodeById(id));
    }

    // Изменяет координаты фигуры
    void MoveShape(ShapeId id, Point position) {
        GetShapeNodeById(id)->second->SetPosition(position);
    }

    // Изменяет размеры фигуры
    void ResizeShape(ShapeId id, Size size) {
        GetShapeNodeById(id)->second->SetSize(size);
    }

    // Возвращает количество фигур
    int GetShapesCount() const {
        return static_cast<int>(shapes_.size());
    }

    // Вывод содержимого холста в поток
    void Print(std::ostream& output) const {
        using namespace std::literals;

        Image image(size_.height, std::string(size_.width, ' '));

        for (const auto& [id, shape] : shapes_) {
            shape->Draw(image);
        }

        output << '#' << std::string(size_.width, '#') << "#\n"sv;
        for (const auto& line : image) {
            output << '#' << line << "#\n"sv;
        }
        output << '#' << std::string(size_.width, '#') << "#\n"sv;
    }

private:
    using Shapes = std::map<ShapeId, std::unique_ptr<Shape>>;

    Shapes::iterator GetShapeNodeById(ShapeId id) {
        auto it = shapes_.find(id);
        if (it == shapes_.end()) {
            throw std::out_of_range("No shape with given ID");
        }
        return it;
    }
    ShapeId InsertShape(std::unique_ptr<Shape> shape) {
        shapes_[current_id_] = std::move(shape);
        return current_id_++;
    }

    Size size_ = {};
    ShapeId current_id_ = 0;
    Shapes shapes_;
};