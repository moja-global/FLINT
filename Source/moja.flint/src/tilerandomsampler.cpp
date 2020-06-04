#include "moja/flint/tilerandomsampler.h"

namespace moja::flint {
const std::vector<std::string> TileRandomSampler::perms = {
    "1234", "1243", "1324", "1342", "1423", "1432", "2134", "2143", "2314", "2341", "2413", "2431",
    "3124", "3142", "3214", "3241", "3412", "3421", "4123", "4132", "4213", "4231", "4312", "4321"};

void TileRandomSampler::construct_addresses() {
   const uint32_t x_size = spatial_location_info_->_cellCols;
   const uint32_t y_size = spatial_location_info_->_cellRows;

   std::string address_string; /* char string representation of the address*/

   const auto last_morton = morton::encode({x_size - 1, y_size - 1});
   const auto morton_str = morton::morton_to_string(last_morton);
   const int n_levels = int(morton_str.size());

   address_string.assign(n_levels, ' ');
   cell_addresses_.clear();
   cell_addresses_.reserve(x_size * y_size);

   /* go through each cell */
   for (uint32_t cell_y = 0; cell_y < y_size; cell_y++) {
      for (uint32_t cell_x = 0; cell_x < x_size; cell_x++) {
         int x = cell_x;
         int y = cell_y;
         /* build the address */
         for (auto lev = n_levels - 1; lev >= 0; --lev) {
            const auto address = 2 * abs(x % 2) + abs(y % 2) + 1;

            if ((x % 2) == -1) {
               x = (x / 2) - 1;
            } else {
               x = x / 2;
            }
            if ((y % 2) == -1) {
               y = (y / 2) - 1;
            } else {
               y = y / 2;
            }

            /* convert the address to a char */
            switch (address) {
               case 1:
                  address_string[lev] = '1';
                  break;
               case 2:
                  address_string[lev] = '2';
                  break;
               case 3:
                  address_string[lev] = '3';
                  break;
               case 4:
                  address_string[lev] = '4';
                  break;
            }
         }
         const auto morton = morton::encode({uint32_t(cell_x), uint32_t(cell_y)});
         cell_addresses_.emplace_back(CellAddress{morton, address_string});
      }
   }
}

const std::string& TileRandomSampler::generate_permutation() {
   std::uniform_int_distribution<unsigned> uniform_distribution(0, 23);  // define the range
   const auto index = uniform_distribution(spatial_location_info_->_engBlock);
   return perms[index];
}

void TileRandomSampler::process_level(uint32_t level, uint32_t n_levels, std::vector<address_node>& addresses) {
   std::vector<col_char_node> col_char_nodes;

   /* check level */
   if (level >= n_levels) return;

   /* generate new perm for this level */
   auto perm = generate_permutation();

   /* create the list of the characters for this column level from the */
   /* sent address list */
   col_char_nodes.reserve(addresses.size());
   for (auto& address_node : addresses) {
      col_char_nodes.emplace_back(col_char_node{address_node.address[level], address_node.address});
   }

   /* create list of all the addresses that have a '1' at the current */
   /* column level */
   std::vector<address_node> address_nodes;
   for (auto& char_node : col_char_nodes) {
      if (char_node.col_char == '1') {
         address_nodes.emplace_back(address_node{char_node.address});
      }
   }

   /* if list is not empty call process_level again and then modify the addresses */
   /* based on the current perm */
   if (!address_nodes.empty()) {
      process_level(level + 1, n_levels, address_nodes);
      for (auto& address_node : address_nodes) {
         address_node.address[level] = perm[0];
      }
   }
   address_nodes.clear();

   /* create list of all the addresses that have a '2' at the current */
   /* column level */
   for (auto& char_node : col_char_nodes) {
      if (char_node.col_char == '2') {
         address_nodes.emplace_back(address_node{char_node.address});
      }
   }
   /* if list is not empty call process_level again and then modify the addresses */
   /* based on the current perm */
   if (!address_nodes.empty()) {
      process_level(level + 1, n_levels, address_nodes);
      for (auto& address_node : address_nodes) {
         address_node.address[level] = perm[1];
      }
   }
   address_nodes.clear();

   /* create list of all the addresses that have a '3' at the current */
   /* column level */
   for (auto& char_node : col_char_nodes) {
      if (char_node.col_char == '3') {
         address_nodes.emplace_back(address_node{char_node.address});
      }
   }
   /* if list is not empty call process_level again and then modify the addresses */
   /* based on the current perm */
   if (!address_nodes.empty()) {
      process_level(level + 1, n_levels, address_nodes);
      for (auto& address_node : address_nodes) {
         address_node.address[level] = perm[2];
      }
   }
   address_nodes.clear();

   /* create list of all the addresses that have a '4' at the current */
   /* column level */
   for (auto& char_node : col_char_nodes) {
      if (char_node.col_char == '4') {
         address_nodes.emplace_back(address_node{char_node.address});
      }
   }
   /* if list is not empty call process_level again and then modify the addresses */
   /* based on the current perm */
   if (!address_nodes.empty()) {
      process_level(level + 1, n_levels, address_nodes);
      for (auto& address_node : address_nodes) {
         address_node.address[level] = perm[3];
      }
   }
}

void TileRandomSampler::randomize_addresses() {
   if (cell_addresses_.empty()) return;
   std::vector<address_node> nodes;
   for (auto& address : cell_addresses_) {
      nodes.emplace_back(address_node{address.address.data()});
   }
   ///* start the algorithm */
   process_level(0, uint32_t(cell_addresses_[0].address.size()), nodes);
}

std::vector<TileRandomSampler::HeirachicalNode> TileRandomSampler::hierarchical_order() const {
   std::vector<HeirachicalNode> nodes;
   int idx = 0;
   for (auto& address : cell_addresses_) {
      std::string rev_address(address.address);
      std::reverse(rev_address.begin(), rev_address.end());
      nodes.emplace_back(HeirachicalNode{idx, address.morton, address.address, rev_address});
      idx++;
   }
   return nodes;
}

std::vector<TileRandomSampler::HeirachicalNode> TileRandomSampler::sorted_hierarchical_order() const {
   std::vector<HeirachicalNode> nodes;
   int idx = 0;
   for (auto& address : cell_addresses_) {
      std::string rev_address(address.address);
      std::reverse(rev_address.begin(), rev_address.end());
      nodes.emplace_back(HeirachicalNode{idx, address.morton, address.address, rev_address});
      idx++;
   }
   std::sort(nodes.begin(), nodes.end(),
             [](const HeirachicalNode& a, const HeirachicalNode& b) -> bool { return a.rev_address > b.rev_address; });
   return nodes;
}

const std::vector<TileRandomSampler::CellAddress>& TileRandomSampler::addresses() const { return cell_addresses_; }

}  // namespace moja::flint
