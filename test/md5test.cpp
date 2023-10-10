#include "md5.h"
#include <gtest/gtest.h>

TEST(MD5, OneBlockSizeInput)
{
   xkbeyer::md5 md5sum;
   md5sum.process("The quick");
   auto result = md5sum.getHash();
   std::string expected = "912d57cea222bc1730dd531b9d6afbb6";
   EXPECT_EQ(expected, result);

   md5sum.process("The quick brown fox jumps over the lazy dog");
   result = md5sum.getHash();
   expected = "9e107d9d372bb6826bd81d3542a419d6";
   EXPECT_EQ(expected, result);
}

TEST(MD5, TwoBlockSizeInput)
{
   // Input has more then 64 characters (one block size 512 bit).
   xkbeyer::md5 md5sum;
   md5sum.process("The buffer contains exact 64 characters, in order to check the algorithm.");
   auto result = md5sum.getHash();
   auto expected = "faa2293d4a2ffecf402242a37d43ce99";
   EXPECT_EQ(expected, result);
}

TEST(MD5, Input_is_exact_63_characters)
{
   //// Input is exact 63 characters (one block size 512 bit).
   xkbeyer::md5 md5sum;
   md5sum.process("The buffer contains exact 63 characters, in order to check the.");
   auto result = md5sum.getHash();
   auto expected = "3d842d0f20f173cf88d73c5a205edc14";
   EXPECT_EQ(expected, result);
}

TEST(MD5, Input_is_exact_64_characters)
{
   //// Input is exact 64 characters (one block size 512 bit).
   xkbeyer::md5 md5sum;
   md5sum.process("The buffer contains exact 64 characters, in order to check the..");
   auto result = md5sum.getHash();
   auto expected = "2123c906547354e0bbe08274df77fae9";
   EXPECT_EQ(expected, result);
}

TEST(MD5, Input_is_more_than_64_characters)
{
   xkbeyer::md5 md5sum;
   md5sum.process("The buffer contains more than 64 characters, in order to check the algo with 2 blocks.");
   auto result = md5sum.getHash();
   auto expected = "22263d2c79c2d8f9decf3087303382e6";
   EXPECT_EQ(expected, result);
}