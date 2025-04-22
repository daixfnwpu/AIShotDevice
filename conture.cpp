#include "appconfig.h"

RGB rgblm = {80, 180, 100, 185, 20, 80};
Line topLine;
Line bottomLine;
RubberDirection rd;
extern bool firsttimerun;
int FIXWIDTHCAL= 100; //用于优化查找像素，只查找图像中的特定的某一部分。
bool findedConture = false; //用于优化查找像素，只查找图像中的特定的某一部分。
int FIEXINFATE = 20; //用于在第一次测试数据的基础上往外扩展20个像素，作为图像处理区域；
Line linearRegression(Point points[], int size) {
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int N = size;
    for (int i = 0; i < size; i++) {
        Point point = points[i];
        sumX += point.x;
        sumY += point.y;
        sumXY += point.x * point.y;
        sumX2 += point.x * point.x;
    }

    double a = (N * sumXY - sumX * sumY) / (N * sumX2 - sumX * sumX);
    double b = (sumY * sumX2 - sumX * sumXY) / (N * sumX2 - sumX * sumX);

    return {a, b};
}

void calculateResiduals(Point points[], int size, const Line& line, double residuals[]) {
    for (int i = 0; i < size; ++i) {
        double yPred = line.slope * points[i].x + line.intercept;
        residuals[i] = abs(points[i].y - yPred);
    }
}

double calculateMean(double values[], int size) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += values[i];
    }
    return sum / size;
}

double calculateStdDev(double values[], int size, double mean) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += pow(values[i] - mean, 2);
    }
    return sqrt(sum / size);
}

int removeOutliers(Point points[], int size, double residuals[], double threshold, Point filteredPoints[]) {
    int j = 0;
    for (int i = 0; i < size; ++i) {
        if (residuals[i] <= threshold) {
            filteredPoints[j++] = points[i];
        }
    }
    return j;
}

bool color_similar(Pixel p, RGB b) {
    return (p.r > rgblm.rmin && p.r < rgblm.rmax) && (p.g > rgblm.gmin && p.g < rgblm.gmax) && (p.b > rgblm.bmin && p.b < rgblm.bmax);
}



// 计算两条直线的交点

//通过该交点的位置，还可以测算皮筋的拉伸长度。
Point findIntersection(Line l1, Line l2) {
    Point p;
    p.x = (l2.intercept - l1.intercept) / (l1.slope - l2.slope);
    p.y = l1.slope * p.x + l1.intercept;
    return p;
}

// 计算对称轴的斜率
double findSymmetryAxisSlope(Line l1, Line l2) {
     return (l1.slope + l2.slope) / (1 - l1.slope * l2.slope);
  
}

// 计算对称轴的截距
double findSymmetryAxisIntercept(double slope, Point intersection) {
    return intersection.y - slope * intersection.x;
}


float perpendicularDistance(Line axisLine, Point  eye) {
    float Px = eye.x;
    float Py = eye.y;
    float B = 1;
    float C = -(axisLine.intercept);  
    float A =  -(axisLine.slope);

    // 计算直线的垂直距离公式的分子部分
    float numerator = std::abs(A * Px + B * Py + C);
    
    // 计算直线的垂直距离公式的分母部分
    float denominator = std::sqrt(A * A + B * B);
    
    // 计算垂直距离
    float distance = numerator / denominator;
    
    return distance;
}


float  perpendicularDistanceByAxis(Point eye,Line l1,Line l2){
    Line axisLine ;
    axisLine.slope = findSymmetryAxisSlope(l1,l2);
    Point section = findIntersection(l1,l2);
    axisLine.intercept = findSymmetryAxisIntercept(axisLine.slope,section);    
    return perpendicularDistance(axisLine,eye);
}



int pixels[HEIGHT][5];
Point points[4][HEIGHT];

std::tuple<RubberDirection,Line,Line> linearLines(Point* indeces, int size, int width, int height) {
    int absdiff[4];
    int intercept[4];
    for (int i = 0; i < HEIGHT; i++) {
        pixels[i][0] = 0;
        pixels[i][1] = -1;
        pixels[i][2] = -1;
        pixels[i][3] = -1;
        pixels[i][4] = -1;
    }

    for (int i = 0; i < size; i++) {
        int y = indeces[i].y;
        int index_y = pixels[y][0];
        if (index_y == 4) continue;
        pixels[y][index_y + 1] = indeces[i].x;
        pixels[y][0]++;
    }


    for (int j = 1; j < 5; j++) {
        int ysize = 0;
        for (int i = 0; i < HEIGHT; i++) {
            if (pixels[i][0] == 4) {
                Point p;
                p.x = pixels[i][j];
                p.y = i;
                points[j - 1][ysize] = p;
                ysize++;
            }
        }
        Point* linearPoints = points[j - 1];
        Line bestFitLine = linearRegression(linearPoints, ysize);

        double residuals[ysize];
        calculateResiduals(linearPoints, ysize, bestFitLine, residuals);

        double meanResidual = calculateMean(residuals, ysize);

        double threshold = meanResidual + 2 * calculateStdDev(residuals, ysize, meanResidual);

        Point filteredPoints[ysize];
        int filteredSize = removeOutliers(linearPoints, ysize, residuals, threshold, filteredPoints);

        Line filteredBestFitLine = linearRegression(filteredPoints, filteredSize);
        // Serial.print("Regression again Line 's a = ");
        // Serial.print(filteredBestFitLine.slope);
        // Serial.print("; b = ");
        // Serial.println(filteredBestFitLine.intercept );
        absdiff[j - 1] = filteredBestFitLine.slope;
        intercept[j - 1] = filteredBestFitLine.intercept ;
    }

    int a1_a4 = abs(absdiff[0]) - abs(absdiff[3]);
    float tmp_rad1 = atan(abs(absdiff[0]));
    float angle_deg1 = degrees(tmp_rad1);

    float tmp_rad4 = atan(abs(absdiff[3]));
    float angle_deg4 = degrees(tmp_rad4);
    int needadjust = angle_deg1 - angle_deg4;
    // Serial.print("angle difference is : ");
    // Serial.print(needadjust);
    // Serial.print(" C ,need adjust the angle of shot ");
    // Serial.println(needadjust / 2);

    int a2_a3 = abs(absdiff[1]) - abs(absdiff[2]);
    RubberDirection rd;
    if (a1_a4 > 0) {
        rd.top = a1_a4;
    } else {
        rd.down = a1_a4;
    }

    topLine.slope = absdiff[0];
    topLine.intercept  = intercept[0];

    bottomLine.slope = absdiff[3];
    bottomLine.intercept  = intercept[3];
    free(indeces);
    
    return {rd,topLine,bottomLine};
}


Image* loadImage(uint8_t * row_pointers) {

    Image * img = new Image();
    img->width = IMAGE_WIDTH;
    img->height=IMAGE_HEIGHT ;
    int totalPixels = img->width * img->height;
    
   for (int i = 0; i < IMAGE_HEIGHT; ++i) {
      for(int j = 0 ; j < IMAGE_WIDTH; j++){
        img->pixels[i][j].r = row_pointers[i * 3];
        img->pixels[i][j].g = row_pointers[i * 3 + 1];
        img->pixels[i][j].b = row_pointers[i * 3 + 2];
      }
    }
    return img;
}

std::vector<std::vector<int>> filterColor(const Image* img) {
    std::vector<std::vector<int>> binaryImage(img->height, std::vector<int>(img->width, 0));
    if(findedConture == false)
    {
      for (int y = 0; y < img->height; ++y) {
          for (int x = 0; x < img->width; ++x) {
              const Pixel& p = img->pixels[y][x];
            
              if (color_similar(p,rgblm)) {
                  binaryImage[y][x] = 1;  
              }
          }
      }
    }
    else {
          int topLine_intercept_inflate = topLine.intercept - FIEXINFATE;
          int bottomLine_intercept_infalte = bottomLine.intercept + FIEXINFATE;
          for(int x = 0 ; x < FIXWIDTHCAL; x++) {
              for(int y = topLine.slope*x +topLine_intercept_inflate ; y < bottomLine.slope*x +bottomLine_intercept_infalte;y ++)
              {
                 const Pixel& p = img->pixels[y][x];
                  if (color_similar(p,rgblm)) {
                      binaryImage[y][x] = 1;  
                  }
              }

          }
        // for (int x = 0; x < img->width; ++x) {
        //   for (int y = tmpEreaTop[x] - FIXHEIGHTDIFF ; y <tmpEreaTop[x] + FIXHEIGHTDIFF ; ++y) {
        //     const Pixel& p = img->pixels[y][x];
        //     if (color_similar(p,rgblm)) {
        //         binaryImage[y][x] = 1;  
        //     }
        // }

        // for (int y = tmpEreaBottom[x] - FIXHEIGHTDIFF ; y <tmpEreaBottom[x] + FIXHEIGHTDIFF ; ++y) {
        //     const Pixel& p = img->pixels[y][x];
        //     if (color_similar(p,rgblm)) {
        //         binaryImage[y][x] = 1;  
        //     }
        // }
      //}
    }
    return binaryImage;
}



// 函数：查找图像中的轮廓
std::tuple<bool,Line,Line> findContours(Image *image) {

  int height = image->height;
  int width = image->width;
  auto binaryImage = filterColor(image) ;
  int indiceSize = 0;
    std::vector<Point> _indices;
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (binaryImage[y][x] == 1) {
                if (binaryImage[y - 1][x] == 0 || binaryImage[y + 1][x] == 0 ||
                    binaryImage[y][x - 1] == 0 || binaryImage[y][x + 1] == 0) {
                    // contours[y][x] = 1;  // 轮廓点
                    // 将轮廓点的坐标保存在 _indices 中（假设 _indices 是全局变量）
                    Point p;
                    p.x = x;
                    p.y = y;
                     _indices.push_back(p); // Arduino 不支持 std::vector，需要自行实现或者使用固定大小的数组
                     indiceSize++;

                }
            }
        }
    }
   Point * indices = (Point*)malloc(sizeof(Point)*indiceSize);
   for(int i = 0; i< _indices.size(); i ++)
   {
    indices[i] = _indices[i];
   }
   if(indiceSize > 1000)
  {
    bool findedConture = true;
    const auto& [rd,topLine,bottomLine]=linearLines(indices,indiceSize,width,height);
    if(firsttimerun &&topLine.slope > 0 && bottomLine.slope != 0  )
    {
        // TODO : 这里用于设置，tmpEreaTop，tmpEreaBottom  topLine 和 bottomLine 的交叉位置，在分别往外扩展20%像素，起使位置，x从0 到50个像素。，.reset firsttimerun . 
        firsttimerun = false;
    }
    return { true,topLine,bottomLine};
  } else //TODO: 返回这个是有问题的；
    return {false,Line{0,0},Line{0,0}};
}


// 函数：在图像上描绘形状轮廓
void outlineShape(Pixel image[IMAGE_HEIGHT][IMAGE_WIDTH], int width, const std::vector<std::pair<int, int>>& shape, Pixel outlineColor) {
    for (const auto& [x, y] : shape) {
        image[y][x] = outlineColor;
        Serial.print("outlineColor: x is : ");
        Serial.print(x);
        Serial.print(", y is :");
        Serial.println(y);
        Serial.print("outlineColor: ");
        Serial.print(outlineColor.r);
        Serial.print(", ");
        Serial.print(outlineColor.g);
        Serial.print(", ");
        Serial.println(outlineColor.b);
    }
}


