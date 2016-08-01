/*
 * 16-bits MSX mapper
 * See http://map.grauw.nl/resources/msx_io_ports.php search "16-bit mappers"
 */
#include "CartridgePluginInterface.hh"

#if defined(__linux__)
#define DLLEXPORT
#else
#define DLLEXPORT __declspec(dllexport)
#endif

#define MBYTE (1024 * 1024)

/**
 * 16-bits mapper size
 */
#define MAPPER_SIZE (16*MBYTE)

#define MAPPER_SIZE_MASK (MAPPER_SIZE-1)

#define MAPPER_CTRL_4 0xf8 ///< Memory Mapper segment selection MSB in 16-bit mappers
#define MAPPER_CTRL_5 0xf9 ///< Memory Mapper segment selection MSB in 16-bit mappers
#define MAPPER_CTRL_6 0xfa ///< Memory Mapper segment selection MSB in 16-bit mappers
#define MAPPER_CTRL_7 0xfb ///< Memory Mapper segment selection MSB in 16-bit mappers
#define MAPPER_CTRL_0 0xfc ///< Memory Mapper segment selection LSB
#define MAPPER_CTRL_1 0xfd ///< Memory Mapper segment selection LSB
#define MAPPER_CTRL_2 0xfe ///< Memory Mapper segment selection LSB
#define MAPPER_CTRL_3 0xff ///< Memory Mapper segment selection LSB

/**
 * 16 bits value
 */
typedef union 
{
   struct 
   {
      uint8_t l; ///< 8 bits low byte
      uint8_t h; ///< 8 bits high byte
   } B;
   uint16_t W; ///< 16 bits value 
} ValuePair;

/**
 * MSX 16-bit memory mapper
 */
uint8_t ram[MAPPER_SIZE];

/**
 * 16 bits mapper setting on Z80 page 0,1,2,3
 */
ValuePair map[4];

using namespace plugin;

CartridgePluginInterface *cpi;

class OpenMsxPluginEvent;
OpenMsxPluginEvent *evt;


/**
 * OPENMSX event handler
 */
class OpenMsxPluginEvent : public CartridgePluginEventInterface
{
   /**
    * Power up event
    */
   void OnOpenMsxPowerUp(uint64_t time) override
   {
      cpi->pluginPrintInfo("Power up");
   };

   /**
    * Power down event
    */
   void OnOpenMsxPowerDown(uint64_t time) override
   {
      cpi->pluginPrintInfo("Power down");
   };

   /**
    * Reset event
    */
   void OnOpenMsxReset(uint64_t time) override
   {
      cpi->pluginPrintInfo("Reset");
   };

   /**
    * Z80 IRQ\ = 0 M1\ = 0
    */
   void OnOpenMsxIorqM1(uint64_t time) override
   {
      // None
   };

   /**
    *
    */
   inline uint32_t getIdx(uint16_t address)
   {
      //             Mapper segment
      //      |-------------------------|
      //      |      Z80 page           |      mapper offset (max 16KB)
      //      |    |-----------|        |    |----------------|
      //      v    v           v        v    v                v    
      return ((map[address >> 14].W << 14) | (address & 0x3fff)) & MAPPER_SIZE_MASK;
   }

   /**
    * Read memory event
    * TODO improve performance. See "OnOpenMsxGetReadCacheLine"
    */
   uint8_t OnOpenMsxReadMem(uint16_t address, uint16_t pc, uint64_t time) override
   { 
      return ram[getIdx(address)];
   };

   /**
    * Write memory event
    * TODO improve performance. See "OnOpenMsxGetWriteCacheLine"
    */
   void OnOpenMsxWriteMem(uint16_t address, uint8_t value, uint16_t pc, uint64_t time) override
   {
      ram[getIdx(address)] = value;
   };

   /**
    * Peek memory event
    */
   uint8_t OnOpenMsxPeekMem(uint16_t address) override 
   { 
      return OnOpenMsxReadMem(address, 0, 0);
   };

   /**
    * 
    */
   const uint8_t *OnOpenMsxGetReadCacheLine(uint16_t start) const override
   { 
      return nullptr; 
   };

   /**
    *
    */
   uint8_t *OnOpenMsxGetWriteCacheLine(uint16_t start) const override
   {
      return nullptr;
   };

   /**
    * Z80 output port
    */
   void OnOpenMsxWriteIO(uint16_t port, uint8_t value, uint16_t pc, uint16_t time) override
   {
   };

   /**
    * Z80 input port
    */
   uint8_t OnOpenMsxReadIO(uint16_t port, uint16_t pc, uint16_t time)
   { 
      switch (port & 0xff)
      {
         // Note that reading these registers is not reliable, and should not be done.
         //   |---> Z80 port
         //   |                         |--> Z80 page
         //   v                         v
         case MAPPER_CTRL_7: return map[3].B.h; // MSB Mapper segment for page 3 (#C000-#FFFF)
         case MAPPER_CTRL_6: return map[2].B.h; // MSB Mapper segment for page 2 (#8000-#BFFF)
         case MAPPER_CTRL_5: return map[1].B.h; // MSB Mapper segment for page 1 (#4000-#7FFF)
         case MAPPER_CTRL_4: return map[0].B.h; // MSB Mapper segment for page 0 (#0000-#3FFF)
         case MAPPER_CTRL_3: return map[3].B.l; // LSB Mapper segment for page 3 (#C000-#FFFF)
         case MAPPER_CTRL_2: return map[2].B.l; // LSB Mapper segment for page 2 (#8000-#BFFF)
         case MAPPER_CTRL_1: return map[1].B.l; // LSB Mapper segment for page 1 (#4000-#7FFF)
         case MAPPER_CTRL_0: return map[0].B.l; // LSB Mapper segment for page 0 (#0000-#3FFF)
      }

      return 0xff; 
   };

   /**
    * Peek memory
    */
   uint8_t OnOpenMsxPeekIO(uint16_t port) override
   { 
      return 0xff; 
   };

   /**
    * Global read memory
    */
   void OnOpenMsxGlobalRead(uint16_t address, uint8_t value, uint16_t pc, uint64_t time) override
   {
      // None
   };

   /**
    * Global write memory
    */
   void OnOpenMsxGlobalWrite(uint16_t address, uint8_t value, uint16_t pc, uint64_t time) override
   {
      // None
   };

   /**
    * Global IO read
    */
   void OnOpenMsxGlobalInpRead(uint16_t address, uint8_t value, uint16_t pc, uint64_t time) override
   {
      // None
   };

   /**
    * Global IO write
    */
   void OnOpenMsxGlobalOutpWrite(uint16_t port, uint8_t value, uint16_t pc, uint64_t time) override
   {
      switch (port & 0xff)
      {
      //   |---> Z80 port
      //   |                  |--> Z80 page
      //   v                  v
      case MAPPER_CTRL_7: map[3].B.h = value; break; // Memory Mapper segment selection MSB in 16 bit mappers
      case MAPPER_CTRL_6: map[2].B.h = value; break; // Memory Mapper segment selection MSB in 16 bit mappers
      case MAPPER_CTRL_5: map[1].B.h = value; break; // Memory Mapper segment selection MSB in 16 bit mappers
      case MAPPER_CTRL_4: map[0].B.h = value; break; // Memory Mapper segment selection MSB in 16 bit mappers
      case MAPPER_CTRL_3: map[3].B.l = value; break; // Memory Mapper segment selection LSB in 16 bit mappers
      case MAPPER_CTRL_2: map[2].B.l = value; break; // Memory Mapper segment selection LSB in 16 bit mappers
      case MAPPER_CTRL_1: map[1].B.l = value; break; // Memory Mapper segment selection LSB in 16 bit mappers
      case MAPPER_CTRL_0: map[0].B.l = value; break; // Memory Mapper segment selection LSB in 16 bit mappers
      }
   };

   /**
    * Load plugin event
    */
   bool OnOpenMsxPluginLoad(void) override 
   { 
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_7);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_6);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_5);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_4);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_3);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_2);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_1);
      cpi->pluginRegisterIoGlobalWrite(MAPPER_CTRL_0);

      return true; // Success loading plugin
   }

   /**
    * Unload plugin event
    */
   bool OnOpenMsxPluginUnLoad(void) override
   { 
      return true; // Success unloading plugin
   };
};

/**
 * DLL plugin entry point
 */
extern "C" int32_t DLLEXPORT OpenMsxPluginEntry(
           CartridgePluginInterface *cpi_, long major, long minor)
{
   if(major == plugin::MAJOR)
   {
      cpi = cpi_;

      // Create event handler
      evt = new OpenMsxPluginEvent();

      // Register plugin events
      cpi->RegisterEvents(evt);
   }

   return (major == plugin::MAJOR) ? 0 : -1;
}
