module;

#include <array>
#include <iterator>
#include <memory>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

export module uuid_tool;

#ifndef LIB_EXPORT
#define LIB_EXPORT
#endif

export namespace uuid_tool
{

template <class ElementType, std::size_t Extent>
using span = std::span<ElementType, Extent>;

namespace detail
{

template <typename TChar>
[[nodiscard]]
constexpr inline unsigned char hex2char(const TChar ch) noexcept
{
    if (ch >= static_cast<TChar>('0') && ch <= static_cast<TChar>('9'))
        return static_cast<unsigned char>(ch - static_cast<TChar>('0'));
    if (ch >= static_cast<TChar>('a') && ch <= static_cast<TChar>('f'))
        return static_cast<unsigned char>(10 + ch - static_cast<TChar>('a'));
    if (ch >= static_cast<TChar>('A') && ch <= static_cast<TChar>('F'))
        return static_cast<unsigned char>(10 + ch - static_cast<TChar>('A'));
    return 0;
}

template <typename TChar>
[[nodiscard]]
constexpr inline bool is_hex(const TChar ch) noexcept
{
    return (ch >= static_cast<TChar>('0') && ch <= static_cast<TChar>('9')) ||
           (ch >= static_cast<TChar>('a') && ch <= static_cast<TChar>('f')) ||
           (ch >= static_cast<TChar>('A') && ch <= static_cast<TChar>('F'));
}

template <typename TChar>
[[nodiscard]]
constexpr std::basic_string_view<TChar> to_string_view(const TChar* str) noexcept
{
    if (str)
        return str;
    return {};
}

template <typename StringType>
[[nodiscard]]
constexpr std::basic_string_view<typename StringType::value_type, typename StringType::traits_type> to_string_view(
    const StringType& str) noexcept
{
    return str;
}

class LIB_EXPORT sha1
{
public:
    using digest32_t = uint32_t[5];
    using digest8_t = uint8_t[20];

    static constexpr unsigned int block_bytes = 64;

    [[nodiscard]]
    inline static uint32_t left_rotate(uint32_t value, const size_t count) noexcept
    {
        return (value << count) ^ (value >> (32 - count));
    }

    sha1() { reset(); }

    void reset() noexcept;

    void process_byte(uint8_t octet);

    void process_block(const void* const start, const void* const end);

    void process_bytes(const void* const data, const size_t len);

    const uint32_t* get_digest(digest32_t digest);

    const uint8_t* get_digest_bytes(digest8_t digest);

private:
    void process_block();

private:
    digest32_t m_digest;
    uint8_t m_block[64];
    size_t m_blockByteIndex;
    size_t m_byteCount;
};

template <typename CharT>
inline constexpr CharT empty_guid[37] = "00000000-0000-0000-0000-000000000000";

template <>
inline constexpr wchar_t empty_guid<wchar_t>[37] = L"00000000-0000-0000-0000-000000000000";

template <typename CharT>
inline constexpr CharT guid_encoder[17] = "0123456789abcdef";

template <>
inline constexpr wchar_t guid_encoder<wchar_t>[17] = L"0123456789abcdef";

} // namespace detail

// --------------------------------------------------------------------------------------------------------------------------
// UUID format https://tools.ietf.org/html/rfc4122
// --------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------------
// Field                         NDR Data Type     Octet #  Note
// --------------------------------------------------------------------------------------------------------------------------
// time_low                   unsigned long    0 - 3       The low field of the timestamp.
// time_mid                   unsigned short       4 - 5       The middle field of the timestamp.
// time_hi_and_version        unsigned short       6 - 7       The high field of the timestamp multiplexed with the
// version number. clock_seq_hi_and_reserved    unsigned small     8          The high field of the clock sequence
// multiplexed with the variant. clock_seq_low                unsigned small     9          The low field of the clock
// sequence. node                      character        10 - 15   The spatially unique node identifier.
// --------------------------------------------------------------------------------------------------------------------------
// 0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          time_low                             |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |       time_mid                |         time_hi_and_version   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         node (2-5)                            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// --------------------------------------------------------------------------------------------------------------------------
// enumerations
// --------------------------------------------------------------------------------------------------------------------------

// indicated by a bit pattern in octet 8, marked with N in xxxxxxxx-xxxx-xxxx-Nxxx-xxxxxxxxxxxx
enum class uuid_variant
{
    // NCS backward compatibility (with the obsolete Apollo Network Computing System 1.5 UUID format)
    // N bit pattern: 0xxx
    // > the first 6 octets of the UUID are a 48-bit timestamp (the number of 4 microsecond units of time since 1 Jan
    // 1980 UTC); > the next 2 octets are reserved; > the next octet is the "address family"; > the final 7 octets are a
    // 56-bit host ID in the form specified by the address family
    ncs,

    // RFC 4122/DCE 1.1
    // N bit pattern: 10xx
    // > big-endian byte order
    rfc,

    // Microsoft Corporation backward compatibility
    // N bit pattern: 110x
    // > little endian byte order
    // > formely used in the Component Object Model (COM) library
    microsoft,

    // reserved for possible future definition
    // N bit pattern: 111x
    reserved,
};

// indicated by a bit pattern in octet 6, marked with M in xxxxxxxx-xxxx-Mxxx-xxxx-xxxxxxxxxxxx
enum class uuid_version
{
    none = 0,                // only possible for nil or invalid uuids
    time_based = 1,          // The time-based version specified in RFC 4122
    dce_security = 2,        // DCE Security version, with embedded POSIX UIDs.
    name_based_md5 = 3,      // The name-based version specified in RFS 4122 with MD5 hashing
    random_number_based = 4, // The randomly or pseudo-randomly generated version specified in RFS 4122
    name_based_sha1 = 5,     // The name-based version specified in RFS 4122 with SHA1 hashing
};

// Forward declare uuid & to_string so that we can declare to_string as a friend later.
class uuid;
template <class CharT = char, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
std::basic_string<CharT, Traits, Allocator> to_string(const uuid& id);

// --------------------------------------------------------------------------------------------------------------------------
// uuid class
// --------------------------------------------------------------------------------------------------------------------------
class LIB_EXPORT uuid
{
public:
    using value_type = uint8_t;

    constexpr uuid() noexcept = default;

    uuid(value_type (&arr)[16]) noexcept { std::copy(std::cbegin(arr), std::cend(arr), std::begin(data)); }

    constexpr uuid(const std::array<value_type, 16>& arr) noexcept
        : data{arr}
    {
    }

    explicit uuid(span<value_type, 16> bytes) { std::copy(std::cbegin(bytes), std::cend(bytes), std::begin(data)); }

    template <typename ForwardIterator>
    explicit uuid(ForwardIterator first, ForwardIterator last)
    {
        if (std::distance(first, last) == 16)
            std::copy(first, last, std::begin(data));
    }

    [[nodiscard]]
    constexpr uuid_variant variant() const noexcept;

    [[nodiscard]]
    constexpr uuid_version version() const noexcept;

    [[nodiscard]]
    constexpr bool is_nil() const noexcept;

    void swap(uuid& other) noexcept { data.swap(other.data); }

    [[nodiscard]]
    inline span<const std::byte, 16> as_bytes() const
    {
        return span<const std::byte, 16>(reinterpret_cast<const std::byte*>(data.data()), 16);
    }

    template <typename StringType>
    [[nodiscard]]
    constexpr static bool is_valid_uuid(const StringType& in_str) noexcept
    {
        auto str = detail::to_string_view(in_str);
        bool firstDigit = true;
        size_t hasBraces = 0;
        size_t index = 0;

        if (str.empty())
            return false;

        if (str.front() == '{')
            hasBraces = 1;
        if (hasBraces && str.back() != '}')
            return false;

        for (size_t i = hasBraces; i < str.size() - hasBraces; ++i)
        {
            if (str[i] == '-')
                continue;

            if (index >= 16 || !detail::is_hex(str[i]))
            {
                return false;
            }

            if (firstDigit)
            {
                firstDigit = false;
            }
            else
            {
                index++;
                firstDigit = true;
            }
        }

        if (index < 16)
        {
            return false;
        }

        return true;
    }

    template <typename StringType>
    [[nodiscard]]
    constexpr static std::optional<uuid> from_string(const StringType& in_str) noexcept
    {
        auto str = detail::to_string_view(in_str);
        bool firstDigit = true;
        size_t hasBraces = 0;
        size_t index = 0;

        std::array<uint8_t, 16> data{{0}};

        if (str.empty())
            return {};

        if (str.front() == '{')
            hasBraces = 1;
        if (hasBraces && str.back() != '}')
            return {};

        for (size_t i = hasBraces; i < str.size() - hasBraces; ++i)
        {
            if (str[i] == '-')
                continue;

            if (index >= 16 || !detail::is_hex(str[i]))
            {
                return {};
            }

            if (firstDigit)
            {
                data[index] = static_cast<uint8_t>(detail::hex2char(str[i]) << 4);
                firstDigit = false;
            }
            else
            {
                data[index] = static_cast<uint8_t>(data[index] | detail::hex2char(str[i]));
                index++;
                firstDigit = true;
            }
        }

        if (index < 16)
        {
            return {};
        }

        return uuid{data};
    }

private:
    std::array<value_type, 16> data{{0}};

    friend bool operator==(const uuid& lhs, const uuid& rhs) noexcept;
    friend bool operator<(const uuid& lhs, const uuid& rhs) noexcept;

    template <class Elem, class Traits>
    friend std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& s, const uuid& id);

    template <class CharT, class Traits, class Allocator>
    friend std::basic_string<CharT, Traits, Allocator> to_string(const uuid& id);

    friend std::hash<uuid>;
};

// --------------------------------------------------------------------------------------------------------------------------
// operators and non-member functions
// --------------------------------------------------------------------------------------------------------------------------

[[nodiscard]]
inline bool operator==(const uuid& lhs, const uuid& rhs) noexcept
{
    return lhs.data == rhs.data;
}

[[nodiscard]]
inline bool operator!=(const uuid& lhs, const uuid& rhs) noexcept
{
    return !(lhs == rhs);
}

[[nodiscard]]
inline bool operator<(const uuid& lhs, const uuid& rhs) noexcept
{
    return lhs.data < rhs.data;
}

template <class CharT, class Traits, class Allocator>
[[nodiscard]]
inline std::basic_string<CharT, Traits, Allocator> to_string(const uuid& id)
{
    std::basic_string<CharT, Traits, Allocator> uustr{detail::empty_guid<CharT>};

    for (size_t i = 0, index = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            continue;
        }
        uustr[i] = detail::guid_encoder<CharT>[id.data[index] >> 4 & 0x0f];
        uustr[++i] = detail::guid_encoder<CharT>[id.data[index] & 0x0f];
        index++;
    }

    return uustr;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& s, const uuid& id)
{
    s << to_string(id);
    return s;
}

inline void swap(uuid_tool::uuid& lhs, uuid_tool::uuid& rhs) noexcept
{
    lhs.swap(rhs);
}

// --------------------------------------------------------------------------------------------------------------------------
// namespace IDs that could be used for generating name-based uuids
// --------------------------------------------------------------------------------------------------------------------------

// Name string is a fully-qualified domain name
constexpr uuid uuid_namespace_dns{
    {0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};

// Name string is a URL
constexpr uuid uuid_namespace_url{
    {0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};

// Name string is an ISO OID (See https://oidref.com/, https://en.wikipedia.org/wiki/Object_identifier)
constexpr uuid uuid_namespace_oid{
    {0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};

// Name string is an X.500 DN, in DER or a text output format (See https://en.wikipedia.org/wiki/X.500,
// https://en.wikipedia.org/wiki/Abstract_Syntax_Notation_One)
constexpr uuid uuid_namespace_x500{
    {0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}};

// --------------------------------------------------------------------------------------------------------------------------
// uuid generators
// --------------------------------------------------------------------------------------------------------------------------

export template <typename UniformRandomNumberGenerator>
class basic_uuid_random_generator
{
public:
    using engine_type = UniformRandomNumberGenerator;

    explicit basic_uuid_random_generator(engine_type& gen)
        : generator(&gen, [](auto) {})
    {
    }

    explicit basic_uuid_random_generator(engine_type* gen)
        : generator(gen, [](auto) {})
    {
    }

    [[nodiscard]]
    uuid operator()()
    {
        alignas(uint32_t) uint8_t bytes[16];
        for (int i = 0; i < 16; i += 4)
            *reinterpret_cast<uint32_t*>(bytes + i) = distribution(*generator);

        // variant must be 10xxxxxx
        bytes[8] &= 0xBF;
        bytes[8] |= 0x80;

        // version must be 0100xxxx
        bytes[6] &= 0x4F;
        bytes[6] |= 0x40;

        return uuid{std::begin(bytes), std::end(bytes)};
    }

private:
    std::uniform_int_distribution<uint32_t> distribution;
    std::shared_ptr<UniformRandomNumberGenerator> generator;
};

using uuid_random_generator = basic_uuid_random_generator<std::mt19937>;

class LIB_EXPORT uuid_name_generator
{
public:
    explicit uuid_name_generator(const uuid& namespace_uuid) noexcept
        : m_nsuuid(namespace_uuid)
    {
    }

    template <typename StringType>
    [[nodiscard]]
    uuid operator()(const StringType& name)
    {
        reset();
        process_characters(detail::to_string_view(name));
        return make_uuid();
    }

private:
    void reset();

    template <typename CharT, typename Traits>
    void process_characters(const std::basic_string_view<CharT, Traits> str)
    {
        for (uint32_t c : str)
        {
            m_hasher.process_byte(static_cast<uint8_t>(c & 0xFF));
            if constexpr (!std::is_same_v<CharT, char>)
            {
                m_hasher.process_byte(static_cast<uint8_t>((c >> 8) & 0xFF));
                m_hasher.process_byte(static_cast<uint8_t>((c >> 16) & 0xFF));
                m_hasher.process_byte(static_cast<uint8_t>((c >> 24) & 0xFF));
            }
        }
    }

    [[nodiscard]]
    uuid make_uuid();

private:
    uuid m_nsuuid;
    detail::sha1 m_hasher;
};

} // namespace uuid_tool

export namespace std
{

template <>
struct hash<uuid_tool::uuid>
{
    using argument_type = uuid_tool::uuid;
    using result_type = std::size_t;

    [[nodiscard]]
    result_type operator()(const argument_type& uuid) const
    {
        uint64_t l = static_cast<uint64_t>(uuid.data[0]) << 56 | static_cast<uint64_t>(uuid.data[1]) << 48 |
                     static_cast<uint64_t>(uuid.data[2]) << 40 | static_cast<uint64_t>(uuid.data[3]) << 32 |
                     static_cast<uint64_t>(uuid.data[4]) << 24 | static_cast<uint64_t>(uuid.data[5]) << 16 |
                     static_cast<uint64_t>(uuid.data[6]) << 8 | static_cast<uint64_t>(uuid.data[7]);
        uint64_t h = static_cast<uint64_t>(uuid.data[8]) << 56 | static_cast<uint64_t>(uuid.data[9]) << 48 |
                     static_cast<uint64_t>(uuid.data[10]) << 40 | static_cast<uint64_t>(uuid.data[11]) << 32 |
                     static_cast<uint64_t>(uuid.data[12]) << 24 | static_cast<uint64_t>(uuid.data[13]) << 16 |
                     static_cast<uint64_t>(uuid.data[14]) << 8 | static_cast<uint64_t>(uuid.data[15]);

        if constexpr (sizeof(result_type) > 4)
        {
            return result_type(l ^ h);
        }
        else
        {
            uint64_t hash64 = l ^ h;
            return result_type(uint32_t(hash64 >> 32) ^ uint32_t(hash64));
        }
    }
};

} // namespace std