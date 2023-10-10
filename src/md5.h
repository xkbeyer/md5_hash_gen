#ifndef XKBEYER_MD5_H
#define XKBEYER_MD5_H
#include <cstdint>
#include <string_view>
#include <array>
#include <string>

namespace xkbeyer {

   /**
    * @brief The MD5 hash algorithm.
   */
   class md5 final
   {
   public:
      /**
       * @brief Process the given buffer and calculate the hash value.
       * @param buffer Buffer to process.
      */
      void process(std::string_view buffer) noexcept;

      /**
       * @brief Query the resulting MD5 hash value. The method process has to be run before.
       * @return MD5 hash value as displayable HEX data.
      */
      std::string getHash() noexcept;

   private:
      void update(std::array<std::uint32_t, 16> const& M) noexcept;
      std::array<std::uint32_t, 4> h{{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}};
   };

} // namespace xkbeyer
#endif