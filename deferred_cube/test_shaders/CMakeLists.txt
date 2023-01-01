include(CompileShader)

set(shader_srcs
  g_buffer.vert
  g_buffer.frag
  deferred.vert
  deferred.frag
)
compile_shader(TARGETS ${shader_srcs})