#include "communication.h"
#include <cstring>
using namespace ns3;

int CommunicateWithAIModule::createOrOpenSharedMemory(BlockInfo& info){
  int fd = shm_open(info.name, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
      perror("shm_open");
      return 1;
  }
  if (ftruncate(fd, info.size) == -1) {
      perror("ftruncate");
      close(fd);
      return 1;
  }
  char* shared_memory = static_cast<char*>(mmap(nullptr, info.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory == MAP_FAILED) {
      perror("mmap");
      close(fd);
      return 1;
  }
  info.fd = fd;
  info.sharedMemory = shared_memory;
  return 0;
}

void CommunicateWithAIModule::freeSharedMemory(BlockInfo info){
  // 解除映射
  if (munmap(info.sharedMemory, info.size) == -1) {
    perror("munmap");
  }
  // 关闭文件描述符
  close(info.fd);
  // 删除共享内存对象
  if (shm_unlink(info.name) == -1) {
      perror("shm_unlink");
  }
}

CommunicateWithAIModule::CommunicateWithAIModule(
  Callback<char*> collectNetInfo, 
  Callback<void, std::string> updateRouting
): CollectNetInfo(collectNetInfo), UpdateRouting(updateRouting){
  // open shared memory of data block
  dataBlockInfo = { -1,  DATA_BLOCK_SIZE, "", DATA_BLOCK_NAME};
  if(createOrOpenSharedMemory(dataBlockInfo) != 0){
    return;
  }
  // open shared memory of control block
  ctrlBlockInfo = { -1,  CONTROL_BLOCK_SIZE, "", CONTROL_BLOCK_NAME};
  if(createOrOpenSharedMemory(ctrlBlockInfo) != 0){
    return;
  }
}

CommunicateWithAIModule::~CommunicateWithAIModule(){
  freeSharedMemory(ctrlBlockInfo);
  freeSharedMemory(dataBlockInfo);
}

std::string CommunicateWithAIModule::readSharedMemory(BlockInfo info, int len){
  if (info.sharedMemory == MAP_FAILED) {
    perror("err occurred while reading");
    close(info.fd);
    return "";
  }
  return getSubstring(info.sharedMemory, len);
}

int CommunicateWithAIModule::writeSharedMemory(char* data, BlockInfo info){
  strncpy(info.sharedMemory, data, strlen(data));
}

std::string getSubstring(const char* str, int n) {
  if (str == nullptr) {
    return "";
  }
  int len = strlen(str);
  n = (n < len)? n : len;
  std::string result(str, n);
  return result;
}

char* getPaddedMod(const char* str, const char* mod) {
  size_t length = strlen(str);
  // 格式化
  std::ostringstream oss;
  oss << mod <<"/" << std::setfill('0') << std::setw(8) << length;
  // 将格式化后的字符串复制到动态分配的内存中
  std::string resultStr = oss.str();
  char* result = new char[resultStr.length() + 1];
  strcpy(result, resultStr.c_str());
  return result;
}

int extractNumberAfterSlash(const std::string& input) {
  // 查找 '/' 字符的位置
  size_t slashPos = input.find('/');
  if (slashPos == std::string::npos) {
      // 如果未找到 '/' 字符，输出错误信息并返回 0
      std::cerr << "Error: No '/' found in the input string." << std::endl;
      return 0;
  }
  // 提取 '/' 后面的子字符串
  std::string numberStr = input.substr(slashPos + 1);
  // 去除前导零
  size_t firstNonZero = numberStr.find_first_not_of('0');
  if (firstNonZero != std::string::npos) {
      numberStr = numberStr.substr(firstNonZero);
  } else {
      // 如果全是零，就设为 "0"
      numberStr = "0";
  }
  // 检查提取的子字符串是否只包含数字字符
  for (char c : numberStr) {
      if (!std::isdigit(c)) {
          std::cerr << "Error: Characters after '/' are not all digits." << std::endl;
          return 0;
      }
  }
  // 将提取的子字符串转换为整数
  std::istringstream iss(numberStr);
  int number;
  if (!(iss >> number)) {
      std::cerr << "Error: Failed to convert the string to an integer." << std::endl;
      return 0;
  }
  return number;
}

void CommunicateWithAIModule::Listen(){
  std::string modSlashLen = readSharedMemory(ctrlBlockInfo, 11); // ns/00000013
  std::string mod = modSlashLen.substr(0, 2);
  if(mod == "ns"){
    int len = extractNumberAfterSlash(modSlashLen);
    std::string data = readSharedMemory(dataBlockInfo, len);
    UpdateRouting(data);
    Simulator::Schedule(Seconds(2.0), &CollectAndSend, this);
  }else{
    Simulator::Schedule(MilliSeconds(50), &Listen, this);
  }
  
}

void CommunicateWithAIModule::CollectAndSend(){
  if(!CollectNetInfo.IsNull()){
    char* data = CollectNetInfo();
    writeSharedMemory(data, dataBlockInfo);
    writeSharedMemory(getPaddedMod(data, "ai"), ctrlBlockInfo);
    Simulator::Schedule(MilliSeconds(50), &Listen, this);
  }
}

void CommunicateWithAIModule::Start(){
  Simulator::Schedule(Seconds(0.5), &CollectAndSend, this);
}
