#ifndef MD5_H
#define MD5_H
#include <cstdint>
#include <string_view>
#include <array>

namespace xkbeyer {

   class md5 final
   {
   public:
      void process(std::string_view buffer) noexcept;
      std::string getHash() noexcept;

   private:
      void update(std::array<std::uint32_t, 16> const& M) noexcept;
      std::array<std::uint32_t, 4> h{{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476}};
   };

} // namespace xkbeyer
#endif