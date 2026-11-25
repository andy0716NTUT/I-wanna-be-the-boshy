#pragma once
#include <algorithm>
#include <optional>

namespace Display {
inline constexpr int LogicalWidth = 1280;
inline constexpr int LogicalHeight = 960;
struct Viewport { int x = 0, y = 0, width = 0, height = 0; };
struct Point { float x, y; };
inline Viewport Fit(int width, int height) {
    if (width <= 0 || height <= 0) return {};
    int w, h;
    if (width >= LogicalWidth && height >= LogicalHeight) {
        const int scale = std::min(width / LogicalWidth, height / LogicalHeight);
        w = LogicalWidth * scale;
        h = LogicalHeight * scale;
    } else {
        const int units = std::min(width / 4, height / 3);
        w = units * 4;
        h = units * 3;
    }
    return {(width-w)/2, (height-h)/2, w, h};
}
inline std::optional<Point> ToLogical(const Viewport& v, int drawableHeight, float x, float y) {
    const int top = drawableHeight-v.y-v.height;
    if (v.width <= 0 || v.height <= 0 || x < v.x || x >= v.x+v.width || y < top || y >= top+v.height)
        return std::nullopt;
    return Point{(x-v.x)*LogicalWidth/v.width-LogicalWidth/2.0f,
                 LogicalHeight/2.0f-(y-top)*LogicalHeight/v.height};
}
inline Point ToDrawable(const Viewport& v, int drawableHeight, Point p) {
    return {v.x+(p.x+LogicalWidth/2.0f)*v.width/LogicalWidth,
            drawableHeight-v.y-v.height+(LogicalHeight/2.0f-p.y)*v.height/LogicalHeight};
}
}
