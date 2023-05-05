#include "md5.h"
#include <charconv>
#include <system_error>
#include <memory>
#include <limits>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace xkbeyer { 
   namespace {
      // Constants are the integer part of the sines of integers (in radians) * 2^32.
      std::array<std::uint32_t, 64> const k {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af,
                                  0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                                  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
                                  0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                                  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97,
                                  0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                                  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
      // r specifies the per-round shift amounts
      std::array<std::uint32_t,64> const r {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
                            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
      
#if __cpp_lib_bitops >= 201907L
      #include <bit>
      constexpr std::uint32_t LEFTROTATE(std::uint32_t x, std::uint32_t shf)
      {
         return std::rotl(x, static_cast<std::int32_t>(shf)); // STL takes an int as shift value.
      }

#else
#if defined(_MSC_VER) 
      #include <intrin.h>
      std::uint32_t LEFTROTATE(std::uint32_t x, std::uint32_t shf)
      {
         return _rotl(x, shf);
      }
#elif defined(__GNUC__) && defined(__x86_64__) 
      #include <X86intrin.h>
      constexpr std::uint32_t LEFTROTATE(std::uint32_t x, std::uint32_t shf)
      {
         return _rotl(x, shf);
      }
#else
      constexpr std::uint32_t LEFTROTATE(std::uint32_t x, std::uint32_t shf)
      {
         return ((x << shf) | (x >> (32UL - shf)));
      }
#endif
#endif // c++20
      std::array<char, 64> generatePadBuffer(std::string_view input)
      {
         std::array<char, 64> padBuffer;
         padBuffer.fill(0);
         size_t const copyBytes = input.length() % 64ULL;
         if (copyBytes != 0ULL) {
            size_t const offset = (input.size() / 64ULL) * 64ULL;
            std::copy_n(&input[offset], copyBytes, std::begin(padBuffer));
         }
         padBuffer[copyBytes] = static_cast<char>(0x80);
         return padBuffer;
      }
    
      void addMsgSize(size_t const msgLength, std::array<char, 64>& block)
      {
         // Input length stored in the last 64bit.
         std::uint32_t const lengthInBits = static_cast<std::uint32_t>(msgLength) * 8UL;
         block[56] = static_cast<char>(lengthInBits & 0x000000FFUL);
         block[57] = static_cast<char>((lengthInBits & 0x0000FF00UL) >> 8UL);
         block[58] = static_cast<char>((lengthInBits & 0x00FF0000UL) >> 16UL);
         block[59] = static_cast<char>((lengthInBits & 0xFF000000UL) >> 24UL);
      }

      std::uint32_t getUint32(std::string_view buffer)
      {
         std::uint32_t value{0UL};
         value = static_cast<std::uint32_t>(buffer[0]) & 0xFFUL;
         value += (static_cast<std::uint32_t>(buffer[1]) & 0xFFUL) << 8UL;
         value += (static_cast<std::uint32_t>(buffer[2]) & 0xFFUL) << 16UL;
         value += (static_cast<std::uint32_t>(buffer[3]) & 0xFFUL) << 24UL;
         return value;
      }

      std::array<std::uint32_t, 16> blockOf16Uint32(std::string_view block)
      {
         std::array<std::uint32_t, 16> M{0UL};
         size_t index = 0ULL;
         // Break 512 bit chunk in to 16 32bit pieces.

         for (size_t i = 0ULL; i < (64ULL / 4ULL); ++i) {
            M[i] = getUint32({&block[index], 4u});
            index += 4ULL;
         }
         return M;
      }

   } // namespace

   class BlockReader final
   {
   public:
      BlockReader(std::string_view inputBuffer) : buffer(inputBuffer)
      {
         blocks = buffer.size() / 64ULL + 1ULL; // Blocks have a size of 64 byte.
         bool needAdditionalBlock = ((buffer.size() * 8ULL) % 512ULL) > 448ULL;
         padBlockNo = blocks - 1ULL;
         padBuffer = generatePadBuffer(buffer);
         if (needAdditionalBlock) {
            ++blocks;
            extraBlockNo = blocks - 1ULL;
         } else {
            addMsgSize(buffer.size(), padBuffer);
         }
      }

      std::array<std::uint32_t, 16> getNextBlock() noexcept
      {
         std::array<std::uint32_t, 16> M{0UL};
         // no extra block padBlock is blocks - 1
         // extra block = padBlock is blocks -2, extra block is blocks -1
         if (currentBlock == padBlockNo) {
            M = blockOf16Uint32({&padBuffer[0], 64});
         } else if (currentBlock == extraBlockNo) {
            M.fill(0UL);
            M[14] = static_cast<std::uint32_t>(buffer.size() * 8ULL);
            if (((buffer.size() * 8ULL) % 512ULL) == 0ULL) {
               // Last block is exact 512, so extra buffer starts with 10.....
               M[0] = 0x80;
            }
         } else {
            M = blockOf16Uint32({&buffer[bufferOffset], 64});
            bufferOffset += 4ULL * 16ULL;
         }
         ++currentBlock;
         return M;
      }

      size_t getBlockCount() const noexcept { return blocks; }
   
   private:
      std::string_view buffer;
      size_t currentBlock{0ULL};
      size_t blocks{0ULL};
      size_t padBlockNo{0ULL};
      size_t extraBlockNo{std::numeric_limits<size_t>::max()};
      std::array<char, 64> padBuffer;
      size_t bufferOffset{0ULL};
   };

   void md5::process(std::string_view buffer) noexcept
   {
      h = {{0x67452301UL, 0xefcdab89UL, 0x98badcfeUL, 0x10325476UL}};
      BlockReader reader{buffer};

      for (size_t block = 0ULL; block < reader.getBlockCount(); ++block) {
         auto M = reader.getNextBlock();
         update(M);
      }
   }

   void md5::update(std::array<std::uint32_t, 16> const& M) noexcept
   {
      std::uint32_t a = h[0];
      std::uint32_t b = h[1];
      std::uint32_t c = h[2];
      std::uint32_t d = h[3];
      std::uint32_t f = 0UL;
      std::uint32_t g = 0UL;
      std::uint32_t temp = 0UL;

      for (std::uint32_t i = 0UL; i < 64UL; ++i) {
         if (i < 16UL) {
            f = (b & c) | ((~b) & d);
            g = i;
         } else if (i < 32UL) {
            f = (d & b) | ((~d) & c);
            g = (5UL * i + 1UL) % 16UL;
         } else if (i < 48UL) {
            f = b ^ c ^ d;
            g = (3UL * i + 5UL) % 16UL;
         } else {
            f = c ^ (b | (~d));
            g = (7UL * i) % 16UL;
         }

         temp = d;
         d = c;
         c = b;
         b = b + LEFTROTATE((a + f + k[i] + M[g]), r[i]);
         a = temp;
      }

      // Add this chunk's hash to result so far:
      h[0] += a;
      h[1] += b;
      h[2] += c;
      h[3] += d;
   }

   std::string md5::getHash() noexcept
   {
      // Concatenate h1-4 as hex numbers.
      std::stringstream hash;
      for (auto i = 0UL; i < 4UL; ++i) {
         hash << std::hex << std::setfill('0') << std::setw(2) << (h[i] & 0x000000FFUL) 
            << std::hex << std::setfill('0') << std::setw(2) << ((h[i] & 0x0000FF00UL) >> 8UL) 
            << std::hex << std::setfill('0') << std::setw(2) << ((h[i] & 0x00FF0000UL) >> 16UL) 
            << std::hex << std::setfill('0') << std::setw(2) << ((h[i] & 0xFF000000UL) >> 24UL);
      }
      return hash.str();
   }
} // namespace xkbeyer