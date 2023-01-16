#include "luna-gfx/gfx.hpp"
#include "luna-gfx/ext/ext.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <chrono>

#include "font.hpp"
#include "draw_vert.hpp"
#include "draw_frag.hpp"

luna::gfx::Font font;
luna::gfx::Window window;
luna::gfx::Renderer renderer;
luna::gfx::FramebufferCreator framebuffers;
luna::gfx::BindGroup bind_group;
luna::gfx::Image img;

constexpr auto cGPU = 0;
constexpr auto cClearColors = std::array<float, 4>{0.0f, 0.2f, 0.2f, 1.0f};
bool running = true;

namespace luna {
auto init_graphics_pipeline() -> void {
  //TODO Run on linux and choose a good default there too :)
  constexpr auto file_path = "C:/Windows/Fonts/arial.ttf";
  auto vert_shader = std::vector<uint32_t>(draw_vert, std::end(draw_vert));
  auto frag_shader = std::vector<uint32_t>(draw_frag, std::end(draw_frag));
  font = std::move(luna::gfx::Font(file_path));
  
  auto info = gfx::RendererInfo();
  framebuffers = gfx::FramebufferCreator(cGPU, window.width(), window.height(), {{"Depth", gfx::ImageFormat::Depth}});
  info.render_pass_info = {cGPU, window.width(), window.height(), 
                            {{"Default", //Subpass
                                // Attachments
                                {{"WindowOutput", window.image_views()}, 
                                {"DepthAttachment", framebuffers.views()["Depth"], {1.0f, 1.0f, 1.0f, 1.0f}}}
                             }}};

  info.pipeline_infos["DefaultPipeline"] = {cGPU, {{"vertex", luna::gfx::ShaderType::Vertex, vert_shader}, {"fragment", luna::gfx::ShaderType::Fragment, frag_shader}}};
  info.pipeline_infos["DefaultPipeline"].details.depth_test = true;
  renderer = std::move(gfx::Renderer(info));

  bind_group = renderer.pipeline("DefaultPipeline").create_bind_group();
  bind_group.set(font.atlas(), "font");
}

auto draw_loop() -> void {
  auto string = std::string("Hello world!");
  auto event_handler = gfx::EventRegister();
  auto event_cb = [&string](const gfx::Event& event) {
    constexpr auto cOffsetAmt = 0.01f;
    if(event.type() == gfx::Event::Type::WindowExit) running = false;
    else if(event.type() == gfx::Event::Type::Text) {
      string += std::string(event.string());
    } else if(event.type() == gfx::Event::Type::KeyDown)
      switch(event.key()) {
        case gfx::Key::Backspace: 
          if(!string.empty())string.pop_back(); break;
      default: break;
    };
  };

  event_handler.add(event_cb);
  luna::gfx::enable_text_input();
  while(running) {
    auto& cmd = renderer.next();
    auto vertices = font.vertices(string);
    window.combo_into(cmd);
    window.acquire();
    
    cmd.begin();
    cmd.start_draw(renderer.pass(), window.current_frame());
    cmd.viewport({static_cast<float>(window.width()), static_cast<float>(window.height())});
    cmd.bind(bind_group);

    if(vertices.size() != 0) {
      cmd.draw(vertices);
    }

    cmd.end_draw();
    cmd.end();
    cmd.combo_into(window);
    auto fence = cmd.submit();
    window.present();
    fence.wait();
    luna::gfx::poll_events();
  }
  
  gfx::synchronize_gpu(cGPU);
}
}

auto render_test() -> void {
  auto info = luna::gfx::WindowInfo();
  info.resizable = true;
  info.resize_callback = {[](){luna::init_graphics_pipeline();}};
  window = luna::gfx::Window(info);
  luna::init_graphics_pipeline();
  luna::draw_loop();
}