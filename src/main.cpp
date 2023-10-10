#include "md5.h"
#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv)
{
   if (argc != 2) {
      std::cerr << "Invalid arguments.\n";
      std::cout << "Usage:\n";
      std::cout << "md5 <file>    Input Filename.\n";
      return -1;
   }
   try {
      std::size_t fileSize = std::filesystem::file_size(argv[1]);
      std::ifstream inFile(argv[1], std::ios::binary);
      std::string buffer(static_cast<size_t>(fileSize), '\0');
      inFile.read(buffer.data(), static_cast<std::streamsize>(fileSize));
      xkbeyer::md5 md5sum;
      md5sum.process({buffer.data(), buffer.size()});
      auto result = md5sum.getHash();
      std::cout << result << '\n';
      return 0;
   } catch (std::exception& ex) {
      std::cerr << ex.what();
   } 

   return -2;
}