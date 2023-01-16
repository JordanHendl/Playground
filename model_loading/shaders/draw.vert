#version 450 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive    : enable

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coord;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;
layout (location = 5) in ivec4 in_bone_ids;
layout (location = 6) in vec4 in_weights;

layout(location = 0) out vec3 frag_normal;
layout(location = 1) out vec2 frag_coord;

layout( binding = 10 ) uniform Transformations { 
mat4 model;
} transform;

void main() {
  mat4 model_matrix = transform.model;

  frag_coord = in_tex_coord;
  frag_normal = in_normal;
  gl_Position =  model_matrix * vec4(in_position, 1);
}
