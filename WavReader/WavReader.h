#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <memory>

#include "Sample.h"

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

    bool operator!=(const WavFile& other) const
    {
        return this->datachk_size != other.datachk_size ||
            this->data != other.data;
               
    }


private:
    WavFile() {}

    friend WavFile generateWavFromData(std::vector<std::byte> data);
};

// sample iterator
class iterator {
public:
    iterator& operator+=(int samples)
    {
        this->offset += samples * ((*file).fmtchk_numchannels * (*file).fmtchk_bitspersample / 8);
        assert(this->offset >= 0);
        assert(this->offset <= (*file).datachk_size);
        return *this;
    }

    iterator& operator-=(int samples)
    {
        return operator+=(-samples);
    }

    iterator& operator++()
    {
        return operator+=(1);
    }

    iterator& operator--()
    {
        return operator+=(-1);
    }

    bool operator!= (const iterator& rhs) const
    {
        return this->offset != rhs.offset ||
            this->file != rhs.file;
    }

    Sample operator*() const
    {
        Sample s(this->file, this->offset);
        return s;
    }

private:
    iterator(std::shared_ptr<WavFile> wav, int32_t off) : file{ wav }, offset{ off } {}
    std::shared_ptr<WavFile> file;
    int32_t offset;

    friend iterator begin(std::shared_ptr<WavFile>);
    friend iterator end(std::shared_ptr<WavFile>);
};

iterator begin(std::shared_ptr<WavFile> file);
iterator end(std::shared_ptr<WavFile> file);

/// Generate the WavFile from data.
WavFile generateWavFromData(std::vector<std::byte> data);
