#pragma once
#include <algorithm>
#include <gdal_priv.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace gdalcpp {

/**
 * Exception thrown for all errors in this class.
 */
class gdal_error : public std::runtime_error {
   std::string _dataset;
   std::string _layer;
   std::string _field;
   CPLErr _error;

  public:
   gdal_error(const std::string& message, const CPLErr error, const std::string& driver = "",
              const std::string& dataset = "", const std::string& layer = "", const std::string& field = "")
       : std::runtime_error(message), _dataset(dataset), _layer(layer), _field(field), _error(error) {}

   const std::string& dataset() const noexcept { return _dataset; }

   const std::string& layer() const noexcept { return _layer; }

   const std::string& field() const noexcept { return _field; }

   CPLErr error() const noexcept { return _error; }
};

namespace detail {

struct init_wrapper {
   init_wrapper() noexcept { GDALAllRegister(); }
};  // struct init_wrapper

struct init_library {
   init_library() {
      static init_wrapper iw;
      CPLSetConfigOption("CPL_DEBUG", "YES");
   }
};  // struct init_library

class driver : init_library {
   GDALDriver* driver_;

  public:
   explicit driver(const std::string& driver_name)
       : driver_(GetGDALDriverManager()->GetDriverByName(driver_name.c_str())) {
      if (!driver_) {
         throw gdal_error{std::string{"unknown driver: '"} + driver_name + "'", CE_None, driver_name};
      }
   }

   GDALDriver& get() const noexcept { return *driver_; }
};

struct options {
   explicit options(const std::vector<std::string>& options) : options_(options), ptrs_(new const char*[options.size() + 1]) {
      std::transform(options_.begin(), options_.end(), ptrs_.get(), [&](const std::string& s) { return s.data(); });
      ptrs_[options.size()] = nullptr;
   }

   char** get() const noexcept { return const_cast<char**>(ptrs_.get()); }

  private:
   std::vector<std::string> options_;
   std::unique_ptr<const char*[]> ptrs_;
};

}  // namespace detail

class dataset : detail::init_library {
   struct gdal_dataset_deleter {
      void operator()(GDALDataset* ds) { GDALClose(ds); }
   };

   std::string dataset_name_;
   detail::options options_;
   std::unique_ptr<GDALDataset, gdal_dataset_deleter> dataset_;

  public:
   dataset(const std::string& path,
           const std::vector<std::string>& options = {})
       : dataset_name_(path),
         options_(options),
         dataset_(static_cast<GDALDataset*>(GDALOpenEx(path.c_str(), GDAL_OF_RASTER | GDAL_OF_VERBOSE_ERROR, nullptr,
                                                       options_.get(), nullptr))) {
      if (!dataset_) {
         throw gdal_error{std::string{"failed to open dataset '"} + path + "'", CE_None, path};
      }
   }

   ~dataset() noexcept {}

   const std::string& dataset_name() const noexcept { return dataset_name_; }

   GDALDataset& get() const noexcept { return *dataset_; }
};

}  // namespace gdalcpp
