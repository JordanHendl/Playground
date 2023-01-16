#pragma once
#include "model_loading/model.hpp"
#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include <string_view>
#include <optional>
namespace luna {
namespace db {
auto configure_json(std::string_view db_json_path) -> void;
auto manually_add_image(std::string_view name, std::string_view path) -> void;
auto manually_add_model(std::string_view name, std::string_view path) -> void;
auto image(std::string_view name) ->  std::shared_ptr<gfx::Image>;
auto model(std::string_view name) -> std::shared_ptr<ModelInfo>;
}
}