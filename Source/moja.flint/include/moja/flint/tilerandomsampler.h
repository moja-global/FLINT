#pragma once
#include "moja/flint/_flint_exports.h"

#include "moja/flint/morton.h"
#include "moja/flint/spatiallocationinfo.h"


#include <random>
#include <vector>

namespace moja {
namespace flint {
class SpatialLocationInfo;

class FLINT_API TileRandomSampler {
  public:
   TileRandomSampler(SpatialLocationInfo& spatial_location_info)
       : spatial_location_info_(&spatial_location_info) {}

   struct CellAddress {
      uint64_t morton;
      std::string address; /* address (i.e. "4311", "321324", etc. ) */
   };

   struct HeirachicalNode {
      int idx;
      uint64_t morton;
      std::string address;
      std::string rev_address;
   };

   void construct_addresses();
   void randomize_addresses();
   std::vector<HeirachicalNode> hierarchical_order() const;
   std::vector<HeirachicalNode> sorted_hierarchical_order() const;
   const std::vector<CellAddress>& addresses() const;

private:
   /* node type for the list of addresses */
   struct address_node {
      char* address; /* address (i.e. "4311", "321324", etc. ) */
   };

   struct col_char_node {
      char col_char; /* character found at column position(i.e. 4)*/
      char* address; /* address (i.e. "4311", "321324", etc. ) */
   };

   static const std::vector<std::string> perms;
   const std::string& generate_permutation();
   void process_level(uint32_t level, uint32_t n_levels, std::vector<address_node>& addresses);

   std::vector<CellAddress> cell_addresses_;
   SpatialLocationInfo* spatial_location_info_;
};
}  // namespace flint
}  // namespace moja
