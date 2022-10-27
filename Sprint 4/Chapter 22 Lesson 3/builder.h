#include "wall.h"

class Builder {
public:
    double CalcBricksNeeded(const Wall& wall) const {
		    double height = wall.GetHeight();
		    double width = wall.GetWidth();
		    return width * height * 5;
	  }
};
