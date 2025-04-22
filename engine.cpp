#include "appconfig.h"



double shotheadwidth = 0.025;


struct Position {
    double x;
    double y;
    double vx;
    double vy;
    double t;
};
std::pair<double, double> calyatx_(
    std::vector<Position> poss,
    double x
);
// 计算空气阻力力
double drag_force(double va, double A) {
    return 0.5 * CD_AIR * RHO * A * va * va;
}

// 计算弹道轨迹
std::vector<Position> calplg(double r, double v0, double theta0, double r0) {
    double A = PI * r * r; // 迎风面积 (m^2)
    double m = r0 * 4 * PI * r * r * r * 1000 / 3; // 弹丸质量 (kg) 密度取2.5
    double theta_rad = theta0 * PI / 180.0;
    double v0_x = v0 * std::cos(theta_rad);
    double v0_y = v0 * std::sin(theta_rad);
    double dt = 0.001; // 时间步长 (s)
    
    std::vector<Position> positions;
    positions.push_back({0, 0, v0_x, v0_y, 0});
    
    double x = 0, y = 0, vx = v0_x, vy = v0_y, t = 0;
    
    while (y >= 0) {
        double v = sqrt(vx*vx + vy*vy);
        double ax = -drag_force(vx, A) *vx  /(v* m);
        double ay = vy > 0 ? -G - drag_force(vy, A) *vy / (v*m) : -G + drag_force(vy, A) *vx / (m*v);
        
        vx += ax * dt;
        vy += ay * dt;
        
        x += vx * dt;
        y += vy * dt;
        
        positions.push_back({x, y, vx, vy, t});
        t += dt;
    }
    
    return positions;
}

// 计算斜率和截距,,,重点求这个函数返回
std::tuple<double, double> calculate_slope_intercept(std::pair<double, double> p1, std::pair<double, double> p2) {
    if (p1.first == p2.first) {
        Serial.print("The points are vertical, cannot define a unique slope.");
    }
    
    double slope = (p2.second - p1.second) / (p2.first - p1.first);
    double intercept = p1.second - slope * p1.first;
    
    return {slope, intercept};
}

// 找到两条直线的交点
std::pair<double, double> find_intersection(double m1, double b1, double m2, double b2) {
    if (m1 == m2) {
        return {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()}; // 平行线，没有交点
    }
    
    double x = (b2 - b1) / (m1 - m2);
    Serial.print("x is : ");
    Serial.println(x);
    Serial.print("m1 is : ");
    Serial.println(m1);
    Serial.print("m2 is : ");
    Serial.println(m2);
    Serial.print("b2 is : ");
    Serial.println(b2);
    Serial.print("b1 is : ");
    Serial.println(b1);
    double y = m1 * x + b1;
    Serial.print("y is : ");
    Serial.println(y);

    return {x, y};
}

// 求某条直线的垂直线的斜率
double perpendicular_slope_at_point(double slope) {
    if (slope == 0) {
        return std::numeric_limits<double>::infinity(); // 如果原线斜率为0（水平线），垂直线斜率为正无穷大
    } else if (std::isinf(slope)) {
        return 0; // 如果原线斜率为正无穷大（垂直向上），垂直线斜率为0
    } else {
        return -1 / slope; // 其他情况下，计算垂直线斜率为原线斜率的负倒数
    }
}

// 求某条直线在给定点的垂线方程
std::function<double(double)> perpendicular_line_equation(double slope, double x1, double y1) {
    double perp_slope = perpendicular_slope_at_point(slope);
    return [perp_slope, x1, y1](double x) {
        return perp_slope * (x - x1) + y1;
    };
}

// 求在某条直线上的线段的起始点
std::pair<std::pair<double, double>, std::pair<double, double>> get_segment_endpoints(double x1, double y1, double segment_length, std::function<double(double)> perp_line_eq,double slope) {
    double direction = 1; // 可以根据需要选择方向，1表示向上，-1表示向下
    	if (slope >= 0)
		    direction = 1; // 可以根据需要选择方向，1表示向上，-1表示向下
	    else 
		    direction = -1;
    double x_start = x1;
    double y_start = y1;
    double cos_theta = 1 / std::sqrt(1 + std::pow(slope,2));
    double x_end = (direction * segment_length)* cos_theta;
    double y_end = perp_line_eq(x_end);
    
    return {{x_start, y_start}, {x_end, y_end}};
}

// 创建直线方程
std::function<double(double)> create_line_lambda(double x1, double y1, double x2, double y2) {
    auto [m, b] = calculate_slope_intercept({x1, y1}, {x2, y2});
    return [m, b](double x) {
        return m * x + b;
    };
}

// 找到在弹道上的点
std::pair<double, double> findposbyshotdistance(double arg, std::vector<Position> & poss, double shotdistance) {

    auto yfunbyx = [&](double xval) {
        auto [x, y] = calyatx_(poss, xval);
        return y;
    };
    
    auto equation = [&](double x, double r) {
        double y = yfunbyx(x);
        return std::abs(std::sqrt(x * x + y * y) - r);
    };
    
    double theta_rad = arg * PI / 180.0;
    double init_x_guesss = shotdistance * std::cos(theta_rad);
    double const_guess = 10 * std::cos(theta_rad);
    
    std::vector<double> initial_guesses(100);
    for (int i = 0; i < 100; ++i) {
        initial_guesses[i] = init_x_guesss - const_guess + i * (2 * const_guess / 100);
    }
    
    std::vector<double> solutions;
    int counterfind = 0;
    bool cont = false;
    for (double guess : initial_guesses) {
        if (equation(guess, shotdistance) <= 0.2) {
            if (counterfind > 3) break;
            solutions.push_back(guess);
            counterfind++;
            cont = true;
        } else if (counterfind >= 1 && !cont) {
            break;
        }
    }
    
    if (!solutions.empty()) {
        std::vector<double> y_solutions;
        for (double solution : solutions) {
            y_solutions.push_back(yfunbyx(solution));
        }
        
        double y_value = y_solutions[0];
        double x_value = solutions[0];
        return {x_value, y_value};
    } else {
        Serial.println("findposbyshotdistance before return");
        return {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    }
}

double calshotpointwithargsbymath(double theta0, double eyeToAxisInPixels, double controlOnAxisInPixels, double targetX, 
double targetY, double cameraWithDistance, double cannotSeeDistance, double slingToEye,double shotdoorwidth=0.04, double powerscala = 1) {
    // Convert angle to radians
    double thetaRad = theta0 * M_PI / 180.0;
    double tanTheta = std::tan(thetaRad);
    double cosTheta = std::cos(thetaRad);



    double targetToAxis = (targetY - targetX * tanTheta) * cosTheta;
    int down = 1;

    if (targetToAxis < 0) {
        down = -1;
        targetToAxis = -targetToAxis;
    }

    double targetOnAxisToZeroPoint = targetX / cosTheta + targetToAxis * tanTheta;

    double cfx;
    if (cannotSeeDistance != 0) {
        cfx = controlOnAxisInPixels / cameraWithDistance + cannotSeeDistance;
    } else {
        cfx = slingToEye;
    }
    double eyeX = eyeToAxisInPixels / cameraWithDistance;
    double y = (cfx * targetToAxis + eyeX * targetOnAxisToZeroPoint) / (cfx + targetOnAxisToZeroPoint);
    if (down == -1) {
        targetOnAxisToZeroPoint = targetX / cosTheta - targetToAxis * tanTheta;
        y = (targetToAxis + eyeX) * targetOnAxisToZeroPoint / (targetOnAxisToZeroPoint + cfx) - targetToAxis;
    }

    return y - (shotheadwidth + shotdoorwidth/2);
}


// 计算瞄准点
double calshotpointwithargs(double theta0,std::pair<double, double> &targetPos, double distancehandtoeye, double eyetoaxis, double shotdistance, double shotdoorwidth=0.04, double powerscala = 1) {

    if (targetPos.first != std::numeric_limits<double>::infinity()) {
        double targetx = targetPos.first;
        double targety = targetPos.second;
        double theta_rad = (theta0 * PI) / 180.0;
        double slope_ = std::tan(theta_rad);
        
        auto [x0, y0] = get_segment_endpoints(0, 0, -distancehandtoeye, [slope_](double x) { return slope_ * x; },slope_).second;
        
        auto eyeonaxisline = perpendicular_line_equation(slope_, x0, y0);
        auto perp_slope = perpendicular_slope_at_point(slope_);
        auto [eyex0, eyey0] = get_segment_endpoints(x0, y0, eyetoaxis, eyeonaxisline,perp_slope).second;
        
        auto line_eq = create_line_lambda(eyex0, eyey0, targetx, targety);
        
        double slopegongmen_ = perpendicular_slope_at_point(slope_);
        double interceptgongmen_ = 0;
        
        auto [shotline_slope, shotline_intercept] = calculate_slope_intercept({eyex0, eyey0}, {targetx, targety});
        auto [interact_x, interact_y] = find_intersection(slopegongmen_, interceptgongmen_, shotline_slope, shotline_intercept);
        
      //  print(f"弓门的瞄点为: {interact_x,interact_y}"); # 该瞄点需要调整在在弓门这条直线上的距离；
			 // print(f"在弓门上的位置为：{shotheadwidth + shotdoorwidth/2 - interact_y/math.cos(theta_rad)}");
        double shotheadPos = shotheadwidth + shotdoorwidth/2 - (interact_y/std::cos(theta_rad));
                Serial.print("shotheadwidth is : ");
        Serial.println(shotheadwidth);
                Serial.print("shotdoorwidth is : ");
        Serial.println(shotdoorwidth);
                Serial.print("interact_y is : ");
        Serial.println(interact_y);
                Serial.print("theta_rad is : ");
        Serial.println(theta_rad);
        Serial.print("shotheadPos is : ");
        Serial.println(shotheadPos);
        return shotheadPos;
    } else {
        return 0.0; 
    }
}


// 计算弹丸曲线中与满足特定特性的点；返回在该点的差异大小
std::tuple<double, double, double, double> calintersectatx(
    const std::vector<double>& xvals,
    const std::vector<double>& yvals,
    double x,
    std::function<double(double, double)> fun
) {
    // 寻找y值
    std::vector<double> y_value;
    for (size_t i = 0; i < xvals.size(); ++i) {
        if (xvals[i] >= x - 1e-2 && xvals[i] <= x + 1e-2) {
            y_value.push_back(yvals[i]);
        }
    }
    
    if (y_value.empty()) {
        Serial.print("No y_value found in the specified range.");
    }

    double y_median = y_value[y_value.size() / 2];

    // 寻找x值
    std::vector<double> x_value;
    for (size_t i = 0; i < yvals.size(); ++i) {
        if (yvals[i] == y_median) {
            x_value.push_back(xvals[i]);
        }
    }

    if (x_value.empty()) {
        Serial.print("No x_value found for the given y_median.");
    }

    double x_median = x_value[x_value.size() / 2];

    double linefun_y = fun(x_median, y_median);
    double diff = std::abs(linefun_y - y_median);

    return std::make_tuple(x_median, y_median, linefun_y, diff);
}

// 计算曲线在指定x处的y值
std::pair<double, double> calyatx_(
    std::vector<Position> poss,
    double x
) {
    std::vector<double> absolute_diff(poss.size());
    for (size_t i = 0; i < poss.size(); ++i) {
        absolute_diff[i] = std::abs(poss[i].x - x);
    }

    size_t index_min_diff = std::min_element(absolute_diff.begin(), absolute_diff.end()) - absolute_diff.begin();
    double closest_value_x = poss[index_min_diff].x;
    double closest_value_y = poss[index_min_diff].y;
    return std::make_pair(closest_value_x, closest_value_y);
}

// 计算y值的差异
std::tuple<double, double, double> calydiffatx(
    const std::vector<std::vector<double>>& ps,
    std::function<double(double)> shotlinefun
) {
    std::vector<double> xps(ps.size());
    std::vector<double> yps(ps.size());

    for (size_t i = 0; i < ps.size(); ++i) {
        xps[i] = ps[i][0];
        yps[i] = ps[i][1];
    }

    std::vector<double> ys(xps.size());
    for (size_t i = 0; i < xps.size(); ++i) {
        ys[i] = shotlinefun(xps[i]);
    }

    std::vector<double> absolute_diff(yps.size());
    for (size_t i = 0; i < yps.size(); ++i) {
        absolute_diff[i] = std::abs(yps[i] - ys[i]);
    }

    size_t index_min_diff = std::min_element(absolute_diff.begin(), absolute_diff.end()) - absolute_diff.begin();
    double absolute_diff_min = *std::min_element(absolute_diff.begin(), absolute_diff.end());

    double closest_value_x = xps[index_min_diff];
    double closest_value_y = yps[index_min_diff];

    return std::make_tuple(closest_value_x, closest_value_y, absolute_diff_min);
}


// 用于确认 弹道轨迹和 计算 距离在轨道上的点；r 为弹丸直径，v0为弹丸的初速度，theta0为陀螺仪的读取值，r0为弹丸的密度；shotdistance 为目标距离（红外线测距仪的读取值）
// 输出 目标在弹道上的点；
std::pair<double, double> initdistanceandtrajectory(double r, double v0, double theta0, double r0,double shotdistance){
    std::vector<Position> poss= calplg( r, v0, theta0, r0);
    std::pair<double, double> tpos =  findposbyshotdistance(theta0, poss, shotdistance);
    Serial.print("tpos is: ");
    Serial.println(tpos.first);//
    Serial.println(tpos.second);

    return tpos;
}

double run(std::pair<double, double>  tpos,double theta0,double distancehandtoeye, double eyetoaxis, double shotdistance, double shotdoorwidth=0.04, double powerscala = 1 ) {
  Serial.println("run start");
  double positionrunOnShotHead = calshotpointwithargs(theta0, tpos, 
    distancehandtoeye, eyetoaxis, shotdistance,shotdoorwidth, powerscala);
  Serial.println("calshotpointwithargs end");
  return  positionrunOnShotHead;
}

