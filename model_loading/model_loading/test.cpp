#include "data_manager.hpp"
#include "model.hpp"
#include "render_test.hpp"
#include <iostream>
#include <cassert>
auto main(int argc, char* argv[]) -> int {
  const auto model_path = "G:/My Drive/assets/models/dia-world-flipper/source/VRC_prep_Dia_7.fbx";
  const auto tex_1 = "G:/My Drive/assets/models/dia-world-flipper/textures/Dia_LP_Untextured_2_Dia_Internals_BaseColo.png";
  const auto tex_2 = "G:/My Drive/assets/models/dia-world-flipper/textures/Dia_LP_Untextured_2_Dia_Body_BaseColor_USE.png";
  const auto tex_3 = "G:/My Drive/assets/models/dia-world-flipper/textures/Dia_LP_Untextured_2_Dia_Clothing_BaseColor.png";
  const auto tex_4 = "G:/My Drive/assets/models/dia-world-flipper/textures/Dia_LP_Untextured_2_Dia_HelmetHair_BaseCol.png";
  const auto tex_5 = "G:/My Drive/assets/models/dia-world-flipper/textures/Dia_LP_Untextured_2_Dia_Collar_BaseColor.png";
  luna::db::manually_add_image("Dia_LP_Untextured_2_Dia_Internals_BaseColor", tex_1);
  luna::db::manually_add_image("Dia_LP_Untextured_2_Dia_Body_BaseColor_USE", tex_2);
  luna::db::manually_add_image("Dia_LP_Untextured_Dia_Clothing_BaseColor", tex_3);
  luna::db::manually_add_image("Dia_LP_Untextured_2_Dia_HelmetHair_BaseCol", tex_4);
  luna::db::manually_add_image("Dia_LP_Untextured_2_Dia_Collar_BaseColor", tex_5);
                                
  luna::db::manually_add_model("cool_model", model_path);
  test_rendering();
  return 0;
}