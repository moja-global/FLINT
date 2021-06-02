#pragma once

namespace moja::flint {

enum class FluxType {
   Wildfire = 1,
   Controlled_burning = 2,
   NPP = 3,
   Harvest = 4,
   Decomposition = 5,
   Turnover = 6,
   Plough = 7,
   Thin = 8,
   Plant_trees = 9,
   Plant_crops = 10,
   N_fertiliser = 11,
   Pest = 12,
   Windthrow = 13,
   Lime_application = 14,
   Grazing = 15,
   Irrigation = 16,
   Herbicide = 17,
   Manure = 18,
   Heavy_roll = 19,
   Wetland_drainage = 20,
   Rice = 21,
   Cattle_Grazing = 22,
   Sheep_Grazing = 23,
   Goat_Grazing = 24,
   Unclassified = 25
};

class OperationDataPackage {
  public:
   OperationDataPackage() = default;
   OperationDataPackage(FluxType fluxType) : _fluxType(fluxType) {}
   virtual ~OperationDataPackage() = default;

   std::string to_string() {
      static std::vector<const char*> flux_type_strings = {
          "Wildfire",      "Controlled burning",
          "NPP",           "Harvest",
          "Decomposition", "Turnover",
          "Plough",        "Thin",
          "Plant trees",   "Plant crops",
          "N fertiliser",  "Pest",
          "Windthrow",     "Lime application",
          "Grazing",       "Irrigation",
          "Herbicide",     "Manure",
          "Heavy roll",    "Wetland drainage",
          "Rice",          "Cattle Grazing",
          "Sheep Grazing", "Goat Grazing",
          "Unclassified"};
      return flux_type_strings[static_cast<size_t>(_fluxType)-1];
   }

   FluxType _fluxType;
};

}  // namespace moja::flint
