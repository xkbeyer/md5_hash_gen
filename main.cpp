#include "md5.h"
#include <iostream>

int main(int argc, char **argv)
{
   (void)argc;
   (void)argv;
   xkbeyer::md5 md5sum;
   md5sum.process("The quick");
   auto result = md5sum.getHash();
   std::string expected = "912d57cea222bc1730dd531b9d6afbb6";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }

   md5sum.process("The quick brown fox jumps over the lazy dog");
   result = md5sum.getHash();
   expected = "9e107d9d372bb6826bd81d3542a419d6";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }
   // Input has more then 64 characters (one block size 512 bit).
   md5sum.process("The buffer contains exact 64 characters, in order to check the algorithm.");
   result = md5sum.getHash();
   expected = "faa2293d4a2ffecf402242a37d43ce99";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }

   // Input is exact 63 characters (one block size 512 bit).
   md5sum.process("The buffer contains exact 63 characters, in order to check the.");
   result = md5sum.getHash();
   expected = "3d842d0f20f173cf88d73c5a205edc14";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }
   // Input is exact 64 characters (one block size 512 bit).
   md5sum.process("The buffer contains exact 64 characters, in order to check the..");
   result = md5sum.getHash();
   expected = "2123c906547354e0bbe08274df77fae9";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }

   md5sum.process("The buffer contains more than 64 characters, in order to check the algo with 2 blocks.");
   result = md5sum.getHash();
   expected = "22263d2c79c2d8f9decf3087303382e6";
   if (result == expected) {
      std::cout << "Passed\n";
   } else {
      std::cout << "Failed:" << expected << " vs " << result << '\n';
   }
   return 0;
}