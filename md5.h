#ifndef MD5_H
#define MD5_H
#include <cstdint>
#include <string_view>
#include <array>

namespace crypt {

class md5
{
public:
   void process(std::string_view buffer);
   std::string getHash();

private:
   void update(std::array<std::uint32_t, 16> const& M);
   std::array<std::uint32_t, 4> h{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
};

} // namespace crypt
#endif