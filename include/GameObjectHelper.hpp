//
// Created by andyl on 2025/4/25.
//

#ifndef GAMEOBJECTHELPER_HPP
#define GAMEOBJECTHELPER_HPP
#pragma once

#include <vector>
#include <memory>

#include "spdlog/fmt/bundled/chrono.h"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "MapInfoLoader.hpp"

// 封裝重複建立邏輯
template<typename T>
std::vector<std::shared_ptr<T>> CreateGameObjectsFromMap(
    const std::shared_ptr<MapInfoLoader>& mapLoader,
    Util::Renderer& renderer)
{
    return T::CreateFromMap(mapLoader, renderer);
}

// 通用的清除物件內容（清除圖片並設為不可見）
template<typename T>
void ClearGameObjects(std::vector<std::shared_ptr<T>>& objects)
{
    for (auto& obj : objects) {
        if (obj) {
            obj->SetVisible(false);
            obj->SetDrawable(nullptr);
        }
    }
    objects.clear();
}

// 清除物件並從渲染器中移除
template<typename T>
void ClearGameObjects(std::vector<std::shared_ptr<T>>& objects, Util::Renderer& renderer)
{
    for (auto& obj : objects) {
        if (obj) {
            obj->SetVisible(false);
            obj->SetDrawable(nullptr);
            renderer.RemoveChild(obj);
        }
    }
    objects.clear();
}

template<typename T>
void ClearGameObjects(std::shared_ptr<T>& objects)
{
    objects->SetDrawable(nullptr);
    objects->SetVisible(false);
    objects = nullptr;
}


// 僅清除繪製圖片但保留物件（可用於換地圖）
template<typename T>
void ClearDrawables(std::vector<std::shared_ptr<T>>& objects)
{
    for (auto& obj : objects) {
        obj->SetDrawable(nullptr);
    }
}
#endif //GAMEOBJECTHELPER_HPP
