#ifndef MOJA_FLINT_WRITESYSTEMCONFIG_H_
#define MOJA_FLINT_WRITESYSTEMCONFIG_H_

#include <moja/flint/modulebase.h>

#include <mutex>

namespace moja {
namespace flint {
class SpatialLocationInfo;

// --------------------------------------------------------------------------------------------

/// Example of using this module
///
///	"WriteSystemConfig": {
///		"enabled": true,
///			"library" : "internal.flint",
///			"order" : 23,
///			"settings" : {
///			"output_path": "./Output/Configs",
///				"name" : "point.dblookup",
///				"write_frequency" : 1,
///				"write_outstep_frequency" : 1,
///				"on_notification" : [
///					"PreTimingSequence",
///					"TimingInit",
///					"TimingShutdown",
///					"OutputStep",
///					"Error"
///				],
///				"on_notificationX" : "OutputStep"
///		}
///	}
///
class FLINT_API WriteSystemConfig : public flint::ModuleBase {
  public:
   explicit WriteSystemConfig(std::mutex& fileHandlingMutex)
       : ModuleBase(),
         _fileHandlingMutex(fileHandlingMutex),
         notificationType(OnNotificationType::TimingInit),
         _writeFrequency(0),
         _writeOutstepFrequency(0),
         _writeCount(0),
         _writeCellProcessed(0),
         _writeThisCell(false) {}
   virtual ~WriteSystemConfig() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   // Notification handlers
   void onSystemInit() override;
   void onLocalDomainInit() override;
   void onPreTimingSequence() override;
   void onTimingInit() override;
   void onTimingShutdown() override;
   void onOutputStep() override;
   void onError(std::string msg) override;

   // --- RAII class for file handle
   class FileHandle {
      typedef FILE* ptr;

     public:
      explicit FileHandle(std::string const& name, std::string const& mode = std::string("r"))
          : _wrapped_file(fopen(name.c_str(), mode.c_str())) {}
      ~FileHandle() {
         if (_wrapped_file) fclose(_wrapped_file);
      }
      operator ptr() const { return _wrapped_file; }

     private:
      ptr _wrapped_file;
   };

  private:
   // Mutexes
   std::mutex& _fileHandlingMutex;

   // FlintData
   std::shared_ptr<const SpatialLocationInfo> _spatialLocationInfo;

   // Other
   std::string _name;             // simulation name
   std::string _outputPath;       // output_path
   bool _onNotificationArray[5];  // when to capture the configuration (which notification method) - TimingInit
                                  // [default], TimingShutdown, Error

   enum class OnNotificationType {
      PreTimingSequence = 0,
      TimingInit = 1,
      TimingShutdown = 2,
      OutputStep = 3,
      Error = 4
   };

   // configuration variables
   OnNotificationType notificationType;  // set by string
   UInt32 _writeFrequency;               // adjust the write frequency to every XXX cells, 0 = all
   UInt32 _writeOutstepFrequency;        // which outsteps to output on, 0 = all, or every X

   // internal variables only
   UInt32 _writeCount;  // how many have been written
   UInt32 _writeCellProcessed;
   bool _writeThisCell;

   void WriteConfig(std::string notificationStr) const;

   static int convertNotificationStringToIndex(std::string str) {
      if (str == "PreTimingSequence") return static_cast<int>(OnNotificationType::PreTimingSequence);
      if (str == "TimingInit") return static_cast<int>(OnNotificationType::TimingInit);
      if (str == "TimingShutdown") return static_cast<int>(OnNotificationType::TimingShutdown);
      if (str == "OutputStep") return static_cast<int>(OnNotificationType::OutputStep);
      if (str == "Error") return static_cast<int>(OnNotificationType::Error);

      // Default to
      return (int)OnNotificationType::TimingInit;
   }

   static std::string convertNotificationToString(OnNotificationType notification) {
      switch (notification) {
         case OnNotificationType::PreTimingSequence:
            return "PreTimingSequence";
         case OnNotificationType::TimingInit:
            return "TimingInit";
         case OnNotificationType::TimingShutdown:
            return "TimingShutdown";
         case OnNotificationType::OutputStep:
            return "OutputStep";
         case OnNotificationType::Error:
            return "Error";
      }
      return "TimingInit";
   }
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_WRITESYSTEMCONFIG_H_