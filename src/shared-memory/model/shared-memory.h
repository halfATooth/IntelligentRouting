#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "ns3/core-module.h"
// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * @defgroup shared-memory Description of the shared-memory
 */

namespace ns3
{
struct BlockInfo
{
  int fd; // shm obj
  int size;
  char* sharedMemory;
  const char* name;
};

class CommunicateWithAIModule
{
private:
  const char* DATA_BLOCK_NAME = "/data_memory";
  const char* CONTROL_BLOCK_NAME = "/control_memory";
  int DATA_BLOCK_SIZE = 1024;
  int CONTROL_BLOCK_SIZE = 1024;
  int duration = 10; // seconds
  int interval = 50; // ms
  BlockInfo dataBlockInfo;
  BlockInfo ctrlBlockInfo;
  Callback<std::string> CollectNetInfo;
  Callback<void, std::string> UpdateRouting;

  int createOrOpenSharedMemory(BlockInfo& info);
  void freeSharedMemory(BlockInfo info);
  std::string readSharedMemory(BlockInfo info, int len);
  void CollectAndSend();
  void Listen();
  std::string getSubstring(const char* str, int n);
  void writeSharedMemory(char* shm, std::string data);

public:
  CommunicateWithAIModule(Callback<std::string> CollectNetInfo, Callback<void, std::string> UpdateRouting);
  ~CommunicateWithAIModule();
  void Start();
};

} // namespace ns3

#endif // SHARED_MEMORY_H
