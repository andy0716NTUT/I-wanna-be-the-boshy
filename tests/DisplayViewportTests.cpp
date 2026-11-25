#include "DisplayViewport.hpp"
#include <stdexcept>
#include <cmath>
#include <utility>
int main() {
    const auto check = [](bool ok) { if (!ok) throw std::runtime_error("Display viewport/mouse mismatch"); };
    for (const auto size : {std::pair{1280,960}, {1920,1080}, {2560,1440}, {3840,2160}, {960,540}, {321,241}}) {
        const auto v = Display::Fit(size.first,size.second);
        check(v.width*3 == v.height*4 && v.x >= 0 && v.y >= 0);
        check(v.x+v.width <= size.first && v.y+v.height <= size.second);
        if (size.first >= 1280 && size.second >= 960) check(v.width % 1280 == 0);
        const auto p = Display::ToDrawable(v,size.second,{123,-45});
        const auto roundtrip = Display::ToLogical(v,size.second,p.x,p.y);
        check(roundtrip && std::abs(roundtrip->x-123) < 0.001f && std::abs(roundtrip->y+45) < 0.001f);
        if (v.x > 0) check(!Display::ToLogical(v,size.second,0,size.second/2.0f));
        if (v.y > 0) check(!Display::ToLogical(v,size.second,size.first/2.0f,0));
    }
    const auto fullhd = Display::Fit(1920,1080);
    check(fullhd.x == 320 && fullhd.y == 60 && fullhd.width == 1280 && fullhd.height == 960);
    const auto qhd = Display::Fit(2560,1440);
    check(qhd.x == 640 && qhd.y == 240 && qhd.width == 1280 && qhd.height == 960);
    const auto uhd = Display::Fit(3840,2160);
    check(uhd.x == 640 && uhd.y == 120 && uhd.width == 2560 && uhd.height == 1920);
    check(Display::Fit(0,0).width == 0 && !Display::ToLogical({},0,0,0));
}
