#include <cassert>
#include <string>
#include <iostream>

using namespace std;

class House {
public:
    House(int length, int width, int height)
        : length_(length), width_(width), height_(height)
    {
    }

    int GetLength() const {
        return length_;
    }

    int GetWidth() const {
        return width_;
    }

    int GetHeight() const {
        return height_;
    }

private:
    int length_, width_, height_;
};

class Resources {
public:
    Resources(int brick_count)
        : brick_count_(brick_count)
    {
    }

    void TakeBricks(int count) {
        if (count < 0 || count > brick_count_) throw out_of_range("wrong count");
        brick_count_ -= count;
    }

    int GetBrickCount() const {
        return brick_count_;
    }

private:
    int brick_count_;
};

struct HouseSpecification {
    int length = 0;
    int width = 0;
    int height = 0;
};

class Builder {
public:
    Builder(Resources& resources)
        : resources_(resources)
    {
    }

    House BuildHouse(HouseSpecification specification) {
        const int one_m2_bricks = 32;
        int bricks_need = (specification.height * specification.length
                         + specification.height * specification.width)
                         * 2 * one_m2_bricks;

        if (bricks_need > resources_.GetBrickCount()) throw runtime_error("need more bricks");
        resources_.TakeBricks(bricks_need);

        return { specification.length, specification.width, specification.height };
    }

private:
    Resources& resources_;
};

int main() {
    Resources resources{ 10000 };
    Builder builder1{ resources };
    Builder builder2{ resources };

    House house1 = builder1.BuildHouse(HouseSpecification{ 12, 9, 3 });
    assert(house1.GetLength() == 12);
    assert(house1.GetWidth() == 9);
    assert(house1.GetHeight() == 3);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House house2 = builder2.BuildHouse(HouseSpecification{ 8, 6, 3 });
    assert(house2.GetLength() == 8);
    cout << resources.GetBrickCount() << " bricks left"s << endl;

    House banya = builder1.BuildHouse(HouseSpecification{ 4, 3, 2 });
    assert(banya.GetHeight() == 2);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
}
