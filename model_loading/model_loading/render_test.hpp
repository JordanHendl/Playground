#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include "model_loading/data_manager.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <chrono>
#include <cmath>
#include <cassert>

#include "draw_vert.hpp"
#include "draw_frag.hpp"

struct Vertex {
  luna::vec3 position;
  luna::vec3 normal;
  luna::vec2 tex_coord;
};


struct Transformations {
  luna::mat4 mat;
};

static_assert(sizeof(Vertex) == (sizeof(float) * 8));
static_assert(sizeof(Transformations) == (sizeof(luna::mat4)));

luna::gfx::Window window;
luna::gfx::Renderer renderer;
luna::gfx::FramebufferCreator framebuffers;
luna::gfx::Vector<Transformations> gpu_transforms;
luna::gfx::PerspectiveCamera camera;
luna::gfx::EventRegister event_handler;
std::shared_ptr<luna::Model> model;

std::vector<luna::gfx::BindGroup> bind_groups;

constexpr auto cGPU = 0;
constexpr auto cClearColors = std::array<float, 4>{0.0f, 0.2f, 0.2f, 1.0f};
constexpr auto cBMPImageHeaderOffset = 54;
bool running = true;

namespace luna {
auto init_event_handlers() -> void {
    static auto camera_angles = vec3{0.0f, 90.0f, 0.0f};
    auto event_cb = [&](const gfx::Event& event) {
    constexpr auto camera_speed = 0.1f;
    constexpr auto rotate_speed = 1.0f;
    if(event.type() == gfx::Event::Type::WindowExit) running = false;

    if(event.type() == gfx::Event::Type::KeyDown)
    switch(event.key()) {
      case gfx::Key::W : camera.translate(camera.front() * camera_speed); break;
      case gfx::Key::S : camera.translate(-(camera.front() * camera_speed)); break;
      case gfx::Key::D : camera.translate(camera.right() * camera_speed); break;
      case gfx::Key::A : camera.translate(-(camera.right() * camera_speed)); break;
      case gfx::Key::Space : camera.translate(-(camera.up() * camera_speed)); break;
      case gfx::Key::LShift : camera.translate((camera.up() * camera_speed)); break;

      case gfx::Key::Left : camera_angles.y += rotate_speed; camera.rotate_euler(camera_angles); break;
      case gfx::Key::Right : camera_angles.y -= rotate_speed; camera.rotate_euler(camera_angles); break;
      case gfx::Key::Up : camera_angles.x -= rotate_speed; camera.rotate_euler(camera_angles); break;
      case gfx::Key::Down : camera_angles.x += rotate_speed; camera.rotate_euler(camera_angles); break;
      default: break;
    };
  };
  event_handler.add(event_cb);
  camera.rotate_euler(camera_angles);
}

auto init_graphics_pipeline() -> void {
  auto vert_shader = std::vector<uint32_t>(draw_vert, std::end(draw_vert));
  auto frag_shader = std::vector<uint32_t>(draw_frag, std::end(draw_frag));

  auto info = gfx::RendererInfo();
  framebuffers = gfx::FramebufferCreator(cGPU, window.width(), window.height(), {{"Depth", gfx::ImageFormat::Depth}});
  info.render_pass_info = {cGPU, window.width(), window.height(), 
  {
    {"Default", 
      {
        {"WindowOutput", window.image_views()}, 
        {"DepthAttachment", framebuffers.views()["Depth"], {1.0f, 1.0f, 1.0f, 1.0f}}
      }
    }
  }
  };

  info.pipeline_infos["DefaultPipeline"] = {cGPU, 
    {
      {"vertex", luna::gfx::ShaderType::Vertex, vert_shader}, 
      {"fragment", luna::gfx::ShaderType::Fragment, frag_shader}
    }
  };

  // Extra parameters.
  info.pipeline_infos["DefaultPipeline"].details.depth_test = true;
  info.pipeline_infos["DefaultPipeline"].details.cull_mode = gfx::CullMode::BackFace;
  info.pipeline_infos["DefaultPipeline"].details.winding = gfx::FrontFaceType::CounterClockwise;
  renderer = std::move(gfx::Renderer(info));

  model = luna::db::model("cool_model");
  assert(model);
  gpu_transforms = gfx::Vector<Transformations>(cGPU, 1);
  bind_groups.resize(model->meshes().size());
  auto i = 0u;
  for(auto& mesh : *model) {
    auto& bg = bind_groups[i++];
    bg = renderer.pipeline("DefaultPipeline").create_bind_group();
    bg.set(gpu_transforms, "transform");
    bg.set(*mesh.textures.begin()->second, "frag_texture");
  }
}

auto draw_loop() -> void {
  init_event_handlers();
  constexpr auto cNearPlane = 0.01f;
  constexpr auto cFarPlane = 1000.f;
  constexpr auto cFOV = 90.f;
  auto ratio = static_cast<float>(window.width()) / static_cast<float>(window.height());
  auto projection = luna::perspective(luna::to_radians(90.f), 1280.f / 1024.f, 0.1f, 1000.f);
  while(running) {
    auto start_time = std::chrono::system_clock::now();
    auto& cmd = renderer.next();

    {
      auto container = gpu_transforms.get_mapped_container();
      auto idx = 0;
      for(auto& transform : container) {
        auto view = camera.info().view_matrix;
        auto vp = projection * view;
        transform.mat = vp;
      }
    }
    gpu_transforms.flush(); 
    // Combo next gpu action to the cmd list.
    window.combo_into(cmd);
    window.acquire();
    
    // Draw some stuff...
    cmd.begin();
    cmd.start_draw(renderer.pass(), window.current_frame());
    cmd.viewport({});
    auto i = 0u;
    for(auto& mesh : *model) {
      auto& bg = bind_groups[i++];
      cmd.bind(bg);
      cmd.draw(mesh.vertices, mesh.indices);
    }
    cmd.end_draw();
    cmd.end();

    cmd.combo_into(window);
    auto fence = cmd.submit();
    window.present();
    luna::gfx::poll_events();
  }
  
  gfx::synchronize_gpu(cGPU);
}
}

auto test_rendering() -> void {
  auto info = luna::gfx::WindowInfo();
  info.resizable = true;
  info.resize_callback = {[](){luna::init_graphics_pipeline();}};
  window = luna::gfx::Window(info);
  luna::init_graphics_pipeline();
  luna::draw_loop();
}