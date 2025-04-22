std::tuple<bool,Line> findContours(Image *image);
Image * loadImage(uint8_t * row_pointers);
float perpendicularDistance(Line topLine, Point eye);
float  perpendicularDistanceByAxis(Point eye,Line l1,Line l2);