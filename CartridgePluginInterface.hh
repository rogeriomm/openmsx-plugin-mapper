#ifndef CARTRIDGEPLUGININTERFACE_HH
#define CARTRIDGEPLUGININTERFACE_HH

#include <stdint.h>

#define OPCODE_LDA_MMMM  (uint32_t(1 << 0))
#define OPCODE_POP_AF    (uint32_t(1 << 1))
#define OPCODE_LDHL_MMMM (uint32_t(1 << 2))

namespace plugin
{
   static const long MAJOR = 0; ///< Interface major version
   static const long MINOR = 1; ///< Interface minor version

   class CartridgePluginEventInterface;

   class CartridgePluginInterface
   {
   public:
      virtual void RegisterEvents(CartridgePluginEventInterface *) = 0;
      virtual void pluginInvalidateMemCache(uint16_t start, unsigned size) = 0;
      virtual void pluginPrintWarning(const char *msg, ...) = 0;
      virtual void pluginPrintError(const char *msg, ...) = 0;
      virtual void pluginPrintProgress(const char *msg, ...) = 0;
      virtual void pluginPrintInfo(const char *msg, ...) = 0;
      virtual void pluginReset(void) = 0;
      virtual void pluginPowerDown(void) = 0;
      virtual void pluginPowerUp(void) = 0;
      virtual bool pluginExecute(const char *) = 0;
      virtual void pluginGetConfigValue(const char *name, char *value, int size) = 0;
      virtual void pluginRegisterGlobalRead(uint16_t mask, uint16_t address, uint32_t opcodes) = 0;
      virtual void pluginUnregisterGlobalRead(uint16_t mask, uint16_t address) = 0;
      virtual void pluginRegisterGlobalWrite(uint16_t address) = 0;
      virtual void pluginUnregisterGlobalWrite(uint16_t address) = 0;
      virtual void pluginRegisterIoGlobalRead(uint8_t port) = 0;
      virtual void pluginUnregisterIoGlobalRead(uint8_t port) = 0;
      virtual void pluginRegisterIoGlobalWrite(uint8_t port) = 0;
      virtual void pluginUnregisterIoGlobalWrite(uint8_t port) = 0;
      virtual bool pluginIsM1Cycle(uint16_t address) = 0;
      virtual int  pluginGetCacheLineSize(void) = 0;
      virtual uint16_t pluginGetPC(void) = 0;
   };

   class CartridgePluginEventInterface
   {
   public:
      virtual void     OnOpenMsxPowerUp(uint64_t /*time*/) {};
      virtual void     OnOpenMsxPowerDown(uint64_t /*time*/) {};
      virtual void     OnOpenMsxReset(uint64_t /*time*/) {};
      virtual void     OnOpenMsxIorqM1(uint64_t /*time*/) {};

      virtual uint8_t  OnOpenMsxReadMem(uint16_t /*address*/, uint16_t /*pc*/, uint64_t /*time*/) { return 0; };
      virtual void     OnOpenMsxWriteMem(uint16_t /*address*/, uint8_t /*value*/, uint16_t /*pc*/, uint64_t /*time*/) {};
      virtual uint8_t  OnOpenMsxPeekMem(uint16_t /*address*/) { return 0; };
      virtual const uint8_t* OnOpenMsxGetReadCacheLine(uint16_t /*start*/) const { return nullptr; };

      virtual void     OnOpenMsxWriteIO(uint16_t /*port*/, uint8_t /*value*/, uint16_t /*pc*/, uint16_t /*time*/) {};
      virtual uint8_t  OnOpenMsxReadIO(uint16_t /*port*/, uint16_t /*pc*/, uint16_t /*time*/) { return 0xff; };
      virtual uint8_t  OnOpenMsxPeekIO(uint16_t /*port*/) { return 0xff; };

      virtual void     OnOpenMsxGlobalRead(uint16_t /*address*/, uint8_t /*value*/, uint16_t /*pc*/, uint64_t /*time*/) {};
      virtual void     OnOpenMsxGlobalWrite(uint16_t /*address*/, uint8_t /*value*/, uint16_t /*pc*/, uint64_t /*time*/) {};
      virtual void     OnOpenMsxGlobalInpRead(uint16_t /*address*/, uint8_t /*value*/, uint16_t /*pc*/, uint64_t /*time*/) {};
      virtual void     OnOpenMsxGlobalOutpWrite(uint16_t /*address*/, uint8_t /*value*/, uint16_t /*pc*/, uint64_t /*time*/) {};
      virtual uint8_t *OnOpenMsxGetWriteCacheLine(uint16_t /*start*/) const { return nullptr; };

      virtual bool     OnOpenMsxPluginLoad(void) { return false; };
      virtual bool     OnOpenMsxPluginUnLoad(void) { return false; };
   };

}; //  namespace plugin

#endif
