module;

#include <array>
#include <cstring>

module uuid_tool;

namespace uuid_tool
{
namespace detail
{

void sha1::reset() noexcept
{
    m_digest[0] = 0x67452301;
    m_digest[1] = 0xEFCDAB89;
    m_digest[2] = 0x98BADCFE;
    m_digest[3] = 0x10325476;
    m_digest[4] = 0xC3D2E1F0;
    m_blockByteIndex = 0;
    m_byteCount = 0;
}

void sha1::process_byte(uint8_t octet)
{
    this->m_block[this->m_blockByteIndex++] = octet;
    ++this->m_byteCount;
    if (m_blockByteIndex == block_bytes)
    {
        this->m_blockByteIndex = 0;
        process_block();
    }
}

void sha1::process_block(const void* const start, const void* const end)
{
    const uint8_t* begin = static_cast<const uint8_t*>(start);
    const uint8_t* finish = static_cast<const uint8_t*>(end);
    while (begin != finish)
    {
        process_byte(*begin);
        begin++;
    }
}

void sha1::process_bytes(const void* const data, const size_t len)
{
    const uint8_t* block = static_cast<const uint8_t*>(data);
    process_block(block, block + len);
}

const uint32_t* sha1::get_digest(digest32_t digest)
{
    const size_t bitCount = this->m_byteCount * 8;
    process_byte(0x80);
    if (this->m_blockByteIndex > 56)
    {
        while (m_blockByteIndex != 0)
        {
            process_byte(0);
        }
        while (m_blockByteIndex < 56)
        {
            process_byte(0);
        }
    }
    else
    {
        while (m_blockByteIndex < 56)
        {
            process_byte(0);
        }
    }
    process_byte(0);
    process_byte(0);
    process_byte(0);
    process_byte(0);
    process_byte(static_cast<unsigned char>((bitCount >> 24) & 0xFF));
    process_byte(static_cast<unsigned char>((bitCount >> 16) & 0xFF));
    process_byte(static_cast<unsigned char>((bitCount >> 8) & 0xFF));
    process_byte(static_cast<unsigned char>((bitCount) & 0xFF));

    memcpy(digest, m_digest, 5 * sizeof(uint32_t));
    return digest;
}

const uint8_t* sha1::get_digest_bytes(digest8_t digest)
{
    digest32_t d32;
    get_digest(d32);
    size_t di = 0;
    digest[di++] = static_cast<uint8_t>(d32[0] >> 24);
    digest[di++] = static_cast<uint8_t>(d32[0] >> 16);
    digest[di++] = static_cast<uint8_t>(d32[0] >> 8);
    digest[di++] = static_cast<uint8_t>(d32[0] >> 0);

    digest[di++] = static_cast<uint8_t>(d32[1] >> 24);
    digest[di++] = static_cast<uint8_t>(d32[1] >> 16);
    digest[di++] = static_cast<uint8_t>(d32[1] >> 8);
    digest[di++] = static_cast<uint8_t>(d32[1] >> 0);

    digest[di++] = static_cast<uint8_t>(d32[2] >> 24);
    digest[di++] = static_cast<uint8_t>(d32[2] >> 16);
    digest[di++] = static_cast<uint8_t>(d32[2] >> 8);
    digest[di++] = static_cast<uint8_t>(d32[2] >> 0);

    digest[di++] = static_cast<uint8_t>(d32[3] >> 24);
    digest[di++] = static_cast<uint8_t>(d32[3] >> 16);
    digest[di++] = static_cast<uint8_t>(d32[3] >> 8);
    digest[di++] = static_cast<uint8_t>(d32[3] >> 0);

    digest[di++] = static_cast<uint8_t>(d32[4] >> 24);
    digest[di++] = static_cast<uint8_t>(d32[4] >> 16);
    digest[di++] = static_cast<uint8_t>(d32[4] >> 8);
    digest[di++] = static_cast<uint8_t>(d32[4] >> 0);

    return digest;
}

void sha1::process_block()
{
    uint32_t w[80];
    for (size_t i = 0; i < 16; i++)
    {
        w[i] = static_cast<uint32_t>(m_block[i * 4 + 0] << 24);
        w[i] |= static_cast<uint32_t>(m_block[i * 4 + 1] << 16);
        w[i] |= static_cast<uint32_t>(m_block[i * 4 + 2] << 8);
        w[i] |= static_cast<uint32_t>(m_block[i * 4 + 3]);
    }
    for (size_t i = 16; i < 80; i++)
    {
        w[i] = left_rotate((w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]), 1);
    }

    uint32_t a = m_digest[0];
    uint32_t b = m_digest[1];
    uint32_t c = m_digest[2];
    uint32_t d = m_digest[3];
    uint32_t e = m_digest[4];

    for (size_t i = 0; i < 80; ++i)
    {
        uint32_t f = 0;
        uint32_t k = 0;

        if (i < 20)
        {
            f = (b & c) | (~b & d);
            k = 0x5A827999;
        }
        else if (i < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if (i < 60)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }
        uint32_t temp = left_rotate(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = left_rotate(b, 30);
        b = a;
        a = temp;
    }

    m_digest[0] += a;
    m_digest[1] += b;
    m_digest[2] += c;
    m_digest[3] += d;
    m_digest[4] += e;
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////
[[nodiscard]]
constexpr uuid_variant uuid::variant() const noexcept
{
    if ((data[8] & 0x80) == 0x00)
        return uuid_variant::ncs;
    else if ((data[8] & 0xC0) == 0x80)
        return uuid_variant::rfc;
    else if ((data[8] & 0xE0) == 0xC0)
        return uuid_variant::microsoft;
    else
        return uuid_variant::reserved;
}

[[nodiscard]]
constexpr uuid_version uuid::version() const noexcept
{
    if ((data[6] & 0xF0) == 0x10)
        return uuid_version::time_based;
    else if ((data[6] & 0xF0) == 0x20)
        return uuid_version::dce_security;
    else if ((data[6] & 0xF0) == 0x30)
        return uuid_version::name_based_md5;
    else if ((data[6] & 0xF0) == 0x40)
        return uuid_version::random_number_based;
    else if ((data[6] & 0xF0) == 0x50)
        return uuid_version::name_based_sha1;
    else
        return uuid_version::none;
}

[[nodiscard]]
constexpr bool uuid::is_nil() const noexcept
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i] != 0)
            return false;
    return true;
}

void uuid_name_generator::reset()
{
    m_hasher.reset();
    std::byte bytes[16];
    auto nsbytes = m_nsuuid.as_bytes();
    std::copy(std::cbegin(nsbytes), std::cend(nsbytes), bytes);
    m_hasher.process_bytes(bytes, 16);
}

[[nodiscard]]
uuid uuid_name_generator::make_uuid()
{
    detail::sha1::digest8_t digest;
    m_hasher.get_digest_bytes(digest);

    // variant must be 0b10xxxxxx
    digest[8] &= 0xBF;
    digest[8] |= 0x80;

    // version must be 0b0101xxxx
    digest[6] &= 0x5F;
    digest[6] |= 0x50;

    return uuid{digest, digest + 16};
}

} // namespace uuid_tool