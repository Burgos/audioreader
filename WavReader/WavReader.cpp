#include "stdafx.h"

#include <array>
#include <vector>
#include <cstddef>
#include <cassert>

struct WavFile {
    /// Chunk id, RIFF, big endian
    /// Not included in chunk size
    std::array<std::byte, 4> chunk_id;

    /// Chunk size, 4 + (8 * fmt subchunk) + (8 * data sub chunk)
    /// Little endian
    int32_t chunk_size;

    /// Format, contains WAVE.
    std::array<std::byte, 4> format;

    /// This is the format subchunk, describing the
    /// format of the file
    std::array<std::byte, 4> fmtchk_id;

    /// 16 for PCM.
    int32_t fmtchk_size;

    /// Audio format, 1 for uncompressed
    int16_t fmtchk_format;

    /// Number of channels
    int16_t fmtchk_numchannels;

    /// Sample rate
    int32_t fmtchk_samplerate;

    /// Byte rate (SampleRate * NumChannels * BitsPerSample/8)
    int32_t fmtchk_byterate;

    /// block align Number of bytes for one sample
    /// including all channels.
    int16_t fmtchk_blockalign;

    /// Bits per sample
    int16_t fmtchk_bitspersample;

    // Data chunk

    /// data chunk identifier
    std::array<std::byte, 4> datachk_id;

    /// size of the data
    int32_t datachk_size;

    /// Data container
    std::vector<std::byte> data;

    /// Iterator to the data segment.
    decltype(data)::iterator data_it;


    WavFile(WavFile&& rhs) = default;
    WavFile(const WavFile& wf) = default;
    WavFile& operator=(const WavFile& wf) = default;

private:
    WavFile() {}

    template <typename Container>
    friend WavFile generateWavFromData(Container&&);
};

template <typename... Args>
auto arrayOfBytes(Args... args)
{
    return std::array<std::byte, sizeof...(args)>{(std::byte)args..., };
}

template <typename T, typename It1, typename It2>
auto safeConsumeFromIt(It1& it, const It2& end)
{
    static_assert(sizeof(T) > 0);

    if (std::distance(it, end) < sizeof(T))
        throw new std::exception("Not enough data to read.");

    // assume little endian TODO
    auto val = *reinterpret_cast<T*>(*it);
    it += sizeof(T);
    return val;
}

// Use only if you know we have enough data.
template <typename T, typename It1, typename It2>
auto consumeFromIt(It1& it, const It2& end) throw()
{
    static_assert(sizeof(T) > 0);

    // assume little endian TODO
    auto val = *reinterpret_cast<T*>(&*it);
    it += sizeof(T);
    return val;
}

//
/// Generates the WavFile from the passed data.
template <typename Container>
WavFile generateWavFromData(Container&& data)
{
    WavFile file;

    // confirm we have enough data
    if (data.size() < 44)
    {
        throw new std::exception("Not enough data to read.");
    }

    file.data = std::forward<Container>(data);

    auto it = std::begin(file.data);

    std::copy(it,
        it + 4,
        std::begin(file.chunk_id));
    it += 4;
    if (file.chunk_id != arrayOfBytes('R', 'I', 'F', 'F'))
        throw new std::exception("Bad chunk id in the input data.");

    file.chunk_size = consumeFromIt<int32_t>(it, std::end(file.data));

    std::copy(it,
        it + 4,
        std::begin(file.format));
    it += 4;
    if (file.format != arrayOfBytes('W', 'A', 'V', 'E'))
        throw new std::exception("Bad chunk id in the input data.");

    std::copy(it,
        it + 4,
        std::begin(file.fmtchk_id));
    it += 4;
    if (file.fmtchk_id != arrayOfBytes('f', 'm', 't', ' '))
        throw new std::exception("Bad chunk id in the input data.");

    file.fmtchk_size = consumeFromIt<int32_t>(it, std::end(file.data));
    file.fmtchk_format = consumeFromIt<int16_t>(it, std::end(file.data));
    file.fmtchk_numchannels = consumeFromIt<int16_t>(it, std::end(file.data));
    file.fmtchk_samplerate = consumeFromIt<int32_t>(it, std::end(file.data));
    file.fmtchk_byterate = consumeFromIt<int32_t>(it, std::end(file.data));
    file.fmtchk_blockalign = consumeFromIt<int16_t>(it, std::end(file.data));
    file.fmtchk_bitspersample = consumeFromIt<int16_t>(it, std::end(file.data));

    std::copy(it,
        it + 4,
        std::begin(file.datachk_id));
    it = it + 4;
    if (file.datachk_id != arrayOfBytes('d', 'a', 't', 'a'))
        throw new std::exception("Bad chunk id in the input data.");

    file.datachk_size = consumeFromIt<int32_t>(it, std::end(file.data));

    // Iterator now points to the data segment.
    file.data_it = it;

    return file;
}

constexpr std::byte asByte(unsigned char val)
{
    return std::byte{ val };
}

int main()
{
    // test it throws

    try
    {
        std::vector<std::byte> data{ 0 };
        generateWavFromData(data);
        assert(false);
    }
    catch (...)
    {
    }

    try
    {
        std::vector<std::byte> data{ 0 };
        generateWavFromData(std::move(data));
        assert(false);
    }
    catch (...)
    {
    }

    std::vector<std::byte> test_data{ asByte(0x52), asByte(0x49),
        asByte(0x46), asByte(0x46), asByte(0x24), asByte(0x08),
        asByte(0x00), asByte(0x00), asByte(0x57), asByte(0x41),
        asByte(0x56), asByte(0x45), asByte(0x66), asByte(0x6d),
        asByte(0x74), asByte(0x20), asByte(0x10), asByte(0x00),
        asByte(0x00), asByte(0x00), asByte(0x01), asByte(0x00),
        asByte(0x02), asByte(0x00), asByte(0x22), asByte(0x56),
        asByte(0x00), asByte(0x00), asByte(0x88), asByte(0x58),
        asByte(0x01), asByte(0x00), asByte(0x04), asByte(0x00),
        asByte(0x10), asByte(0x00), asByte(0x64), asByte(0x61),
        asByte(0x74), asByte(0x61), asByte(0x00), asByte(0x08),
        asByte(0x00), asByte(0x00), asByte(0x00), asByte(0x00),
        asByte(0x00), asByte(0x00), asByte(0x24), asByte(0x17),
        asByte(0x1e), asByte(0xf3), asByte(0x3c), asByte(0x13),
        asByte(0x3c), asByte(0x14), asByte(0x16), asByte(0xf9),
        asByte(0x18), asByte(0xf9), asByte(0x34), asByte(0xe7),
        asByte(0x23), asByte(0xa6), asByte(0x3c), asByte(0xf2),
        asByte(0x24), asByte(0xf2), asByte(0x11), asByte(0xce),
        asByte(0x1a), asByte(0x0d) };

    auto wavFile{ generateWavFromData(std::move(test_data)) };

    return 0;
}