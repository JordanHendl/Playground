#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include "scene_graph/scene.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <chrono>
#include <cmath>

#include "raw_data/default_image.hpp"
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
luna::gfx::BindGroup bind_group;
luna::gfx::Image img;
luna::gfx::PerspectiveCamera camera;
luna::gfx::EventRegister event_handler;
luna::Scene scene;

std::vector<std::size_t> m_nodes;
std::size_t which_cube_to_move = 1u;

constexpr auto cGPU = 0;
constexpr auto cNumCubes = 32u;
constexpr auto cClearColors = std::array<float, 4>{0.0f, 0.2f, 0.2f, 1.0f};
constexpr auto cBMPImageHeaderOffset = 54;
const std::vector<Vertex> cVertices = {
        // positions          // normals           // texture coords
        {{-0.2f, -0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f}},
        {{ 0.2f, -0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {1.0f,  0.0f}},
        {{ 0.2f,  0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
        {{ 0.2f,  0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {1.0f,  1.0f}},
        {{-0.2f,  0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {0.0f,  1.0f}},
        {{-0.2f, -0.2f,  -0.2f},  {0.0f,  0.0f, -1.0f},  {0.0f,  0.0f}},
        {{-0.2f, -0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},
        {{ 0.2f, -0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {1.0f,  0.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {1.0f,  1.0f}},
        {{-0.2f,  0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {0.0f,  1.0f}},
        {{-0.2f, -0.2f,   0.2f},  {0.0f,  0.0f,  1.0f},  {0.0f,  0.0f}},
        {{-0.2f,  0.2f,   0.2f},  {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
        {{-0.2f,  0.2f,  -0.2f},  {-1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
        {{-0.2f, -0.2f,  -0.2f},  {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{-0.2f, -0.2f,  -0.2f},  {-1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{-0.2f, -0.2f,   0.2f},  {-1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
        {{-0.2f,  0.2f,   0.2f},  {-1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
        {{ 0.2f,  0.2f,  -0.2f},  {1.0f,  0.0f,  0.0f},  {1.0f,  1.0f}},
        {{ 0.2f, -0.2f,  -0.2f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{ 0.2f, -0.2f,  -0.2f},  {1.0f,  0.0f,  0.0f},  {0.0f,  1.0f}},
        {{ 0.2f, -0.2f,   0.2f},  {1.0f,  0.0f,  0.0f},  {0.0f,  0.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {1.0f,  0.0f,  0.0f},  {1.0f,  0.0f}},
        {{-0.2f, -0.2f,  -0.2f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},
        {{ 0.2f, -0.2f,  -0.2f},  {0.0f, -1.0f,  0.0f},  {1.0f,  1.0f}},
        {{ 0.2f, -0.2f,   0.2f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
        {{ 0.2f, -0.2f,   0.2f},  {0.0f, -1.0f,  0.0f},  {1.0f,  0.0f}},
        {{-0.2f, -0.2f,   0.2f},  {0.0f, -1.0f,  0.0f},  {0.0f,  0.0f}},
        {{-0.2f, -0.2f,  -0.2f},  {0.0f, -1.0f,  0.0f},  {0.0f,  1.0f}},
        {{-0.2f,  0.2f,  -0.2f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}},
        {{ 0.2f,  0.2f,  -0.2f},  {0.0f,  1.0f,  0.0f},  {1.0f,  1.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
        {{ 0.2f,  0.2f,   0.2f},  {0.0f,  1.0f,  0.0f},  {1.0f,  0.0f}},
        {{-0.2f,  0.2f,   0.2f},  {0.0f,  1.0f,  0.0f},  {0.0f,  0.0f}},
        {{-0.2f,  0.2f,  -0.2f},  {0.0f,  1.0f,  0.0f},  {0.0f,  1.0f}}
    };

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
      case gfx::Key::Q : if(which_cube_to_move != 1) which_cube_to_move--; break;
      case gfx::Key::E : if(which_cube_to_move < cNumCubes) which_cube_to_move++; break;

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

  info.pipeline_infos["DefaultPipeline"].details.depth_test = true;
  renderer = std::move(gfx::Renderer(info));

  img = gfx::Image({"Default_Image", 0, 1024, 1024}); // Name, gpu, width, height
  gpu_transforms = gfx::Vector<Transformations>(cGPU, cNumCubes);
  bind_group = renderer.pipeline("DefaultPipeline").create_bind_group();
  bind_group.set(gpu_transforms, "transform");
  bind_group.set(img, "cube_texture");
  img.upload(DEFAULT_bmp + cBMPImageHeaderOffset);
}

auto draw_loop() -> void {
  init_event_handlers();
  auto vertices = gfx::Vector<Vertex>(cGPU, cVertices.size());
  vertices.upload(cVertices.data());
  auto rot = 0.0;
  scene.add_node_type("cube");
  // Insert nodes into the scene graph.
  for(auto i = 0u; i < cNumCubes; ++i) {
    if(i == 0u) m_nodes.push_back(scene.add_child(std::to_string(i), {"cube", "", {{static_cast<float>(i) / 4.0f, 0.0f, 0.0f} }}));
    else m_nodes.push_back(scene.add_child(std::to_string(i), {"cube", "", {{static_cast<float>(i) / 4.0f, 0.0f, 0.0f}}}, m_nodes.back()));
  }

  constexpr auto cNearPlane = 0.01f;
  constexpr auto cFarPlane = 1000.f;
  constexpr auto cFOV = 90.f;
  auto ratio = static_cast<float>(window.width()) / static_cast<float>(window.height());
  auto projection = luna::perspective(luna::to_radians(90.f), 1280.f / 1024.f, 0.1f, 1000.f);
  while(running) {
    auto start_time = std::chrono::system_clock::now();
    auto& cmd = renderer.next();
    scene.get(which_cube_to_move).transform().position.y = std::sin(luna::to_radians(rot));
    scene.traverse();

    // Update rotation
    {
      auto container = gpu_transforms.get_mapped_container();
      auto idx = 0;
      for(auto& transform : container) {
        auto view = camera.info().view_matrix;
        auto vp = projection * view;
        auto model = scene.get(m_nodes[idx++]).transform().transform;
        transform.mat = vp * model;
      }
    }

    gpu_transforms.flush(); 
    // Combo next gpu action to the cmd list.
    window.combo_into(cmd);
    window.acquire();
    
    // Draw some stuff...
    cmd.begin();
    cmd.start_draw(renderer.pass(), window.current_frame());
    cmd.bind(bind_group);
    cmd.viewport({});
    cmd.draw(vertices, cNumCubes);
    cmd.end_draw();
    cmd.end();

    // Combo this cmd list submit (sending stuff to the GPU) into the window for it's next GPU operation.
    cmd.combo_into(window);

    // Submit CONSUMES the window combo.
    auto fence = cmd.submit();

    // CONSUMES the cmd combo.
    // Present the window. Since it was combo'd into by the cmd, it will wait on the cmd to finish on the GPU before presenting.
    window.present();

    // Wait for command to finish. Don't have to in realtime, but since this is looping we need to do so.
    luna::gfx::poll_events();

    auto end_time = std::chrono::system_clock::now();
    using in_millis = std::chrono::duration<float, std::milli>;
    auto delta = std::chrono::duration_cast<in_millis>(start_time - end_time);
    rot += (60.f * (delta.count() / 1000.f));
  }
  
  // Before we deconstruct everything, make sure we're done working on the GPU.
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