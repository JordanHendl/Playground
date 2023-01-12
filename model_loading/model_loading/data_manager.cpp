#include "model_loading/data_manager.hpp"
namespace luna {
namespace db {
struct DataManagerData {
};

auto configure_json(std::string_view db_json_path) -> void {

}

auto manually_add_image(std::string_view name, std::string_view path) -> void {

}

auto manually_add_model(std::string_view name, std::string_view path) -> void {

}

auto image(std::string_view name) -> std::optional<gfx::ImageView> {
  return {};
}

auto model(std::string_view name) -> std::optional<Model> {
  return {};
}
}
}