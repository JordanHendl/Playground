#include "model_loading/data_manager.hpp"
#include "model_loading/model.hpp"
#include "raw_data/default_image.hpp"
#include <cassert>
#include <unordered_map>
#include <memory>
#include <iostream>
namespace luna {
namespace db {
constexpr auto cBMPImageHeaderOffset = 54;
constexpr auto NUM_MIPS = 1;
constexpr auto cGPU = 0;
template<typename Type>
struct ItemInfo {
  std::string access_info;
  std::weak_ptr<Type> data;
};
struct DataManagerData {
  std::unordered_map<std::string, ItemInfo<gfx::Image>> images;
  std::unordered_map<std::string, ItemInfo<Model>> models;
  std::shared_ptr<gfx::Image> default_image;

  DataManagerData() {
    this->default_image = std::make_shared<gfx::Image>(gfx::ImageInfo{"Default_Image", cGPU, 1024, 1024});
    this->default_image->upload(DEFAULT_bmp + cBMPImageHeaderOffset);
  }
};

static auto db_data() -> DataManagerData& {
  static auto d = DataManagerData();
  return d;
}

auto configure_json(std::string_view db_json_path) -> void {
  //TODO
}

auto manually_add_image(std::string_view name, std::string_view path) -> void {
  auto& res = db_data();
  auto iter = res.images.find(std::string(name.data()));
  if(iter == res.images.end()) {
    res.images.insert({std::string(name.data()), {std::string(path.data()), {}}});
  }
}

auto manually_add_model(std::string_view name, std::string_view path) -> void {
auto& res = db_data();
  auto iter = res.models.find(std::string(name.data()));
  if(iter == res.models.end()) {
    res.models.insert({std::string(name.data()), {std::string(path.data()), {}}});
  }
}

auto image(std::string_view name) -> std::shared_ptr<gfx::Image> {
  auto& res = db_data();
  auto iter = res.images.find(std::string(name.data()));
  if(iter != res.images.end()) {
    if(iter->second.data.expired()) {
      auto loader = gfx::loadRGBA8(iter->second.access_info.c_str());
      assert(!loader.pixels.empty());
      auto info = gfx::ImageInfo{std::string(name.data()), cGPU,
                             static_cast<std::size_t>(loader.width), static_cast<std::size_t>(loader.height), 1u,
                             gfx::ImageFormat::RGBA8,
                             NUM_MIPS};

      auto image = std::make_shared<gfx::Image>(info);
      image->upload(loader.pixels.data());
      iter->second.data = image;
      return image;
    } else {
      return iter->second.data.lock();
    }
  }

  std::cout << "Could not load item " << name << std::endl;
  // Look up error.
  return res.default_image;
}

auto model(std::string_view name) -> std::shared_ptr<Model> {
  auto& res = db_data();
  auto iter = res.models.find(std::string(name.data()));
  if(iter != res.models.end()) {
    if(iter->second.data.expired()) {
      auto model = load_model_file(iter->second.access_info);
      iter->second.data = model;
      return model;
    } else {
      return iter->second.data.lock();
    }
  }
  // Look up error.
  return {};
}
}
}