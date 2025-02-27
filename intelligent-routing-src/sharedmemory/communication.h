#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "ns3/core-module.h"

using namespace ns3;

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
  Callback<char*> CollectNetInfo;
  Callback<void, std::string> UpdateRouting;

  int createOrOpenSharedMemory(BlockInfo& info);
  void freeSharedMemory(BlockInfo info);
  std::string readSharedMemory(BlockInfo info, int len);
  int writeSharedMemory(char* data, BlockInfo info);
  void CollectAndSend();
  void Listen();

public:
  CommunicateWithAIModule(Callback<char*> CollectNetInfo, Callback<void, std::string> UpdateRouting);
  ~CommunicateWithAIModule();
  void Start();
};
