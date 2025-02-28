#include "shared-memory.h"

namespace ns3
{

int CommunicateWithAIModule::createOrOpenSharedMemory(BlockInfo& info){
  int fd = shm_open(info.name, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    std::cout<< "create shm_open err: " << info.name << std::endl;
    return 1;
  }
  if (ftruncate(fd, info.size) == -1) {
    std::cout<< "create ftruncate err: " << info.name << std::endl;
    close(fd);
    return 1;
  }
  char* shared_memory = static_cast<char*>(mmap(nullptr, info.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory == MAP_FAILED) {
    std::cout<< "create mmap err: " << info.name << std::endl;
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
  Callback<std::string> collectNetInfo, 
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
  printf("memory ready\n");
}

CommunicateWithAIModule::~CommunicateWithAIModule(){
  std::cout << "~CommunicateWithAIModule" << std::endl;
  freeSharedMemory(ctrlBlockInfo);
  freeSharedMemory(dataBlockInfo);
}

std::string CommunicateWithAIModule::readSharedMemory(BlockInfo info, int len){
  if (info.sharedMemory == MAP_FAILED) {
    std::cout<< "read shm_open err: " << info.name << std::endl;
    close(info.fd);
    return "";
  }
  return getSubstring(info.sharedMemory, len);
}

std::string CommunicateWithAIModule::getSubstring(const char* str, int n) {
  if (str == nullptr) {
    return "";
  }
  int len = std::string(str).length();
  n = (n < len)? n : len;
  std::string result(str, n);
  return result;
}

std::string getPaddedMod(std::string str, const char* mod) {
  int length = str.length();
  // 格式化
  std::ostringstream oss;
  oss << mod <<"/" << std::setfill('0') << std::setw(8) << length;
  return oss.str();
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
    Simulator::Schedule(Seconds(5.0), &CommunicateWithAIModule::CollectAndSend, this);
  }else{
    Simulator::Schedule(MilliSeconds(50), &CommunicateWithAIModule::Listen, this);
  }
}

void CommunicateWithAIModule::CollectAndSend(){
  if(!CollectNetInfo.IsNull()){
    std::string data = CollectNetInfo();
    writeSharedMemory(dataBlockInfo.sharedMemory, data);
    writeSharedMemory(ctrlBlockInfo.sharedMemory, getPaddedMod(data, "ai"));
    Simulator::Schedule(MilliSeconds(50), &CommunicateWithAIModule::Listen, this);
  }else{
    printf("CollectNetInfo.IsNull\n");
  }
}

void CommunicateWithAIModule::Start(){
  Simulator::Schedule(Seconds(0.1), &CommunicateWithAIModule::CollectAndSend, this);
}

void CommunicateWithAIModule::writeSharedMemory(char* shm, std::string data){
  std::cout<<"write: "<<data<<std::endl;
  for(int i=0; i<data.length(); i++){
    shm[i] = data[i];
  }
}

} // namespace ns3
