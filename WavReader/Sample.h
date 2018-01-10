#pragma once

#include <utility>
#include <memory>

struct WavFile;

// Sample
struct Sample
{
    struct Channel
    {
        const std::byte& operator[](std::size_t idx) const;

    private:
        const Sample& s;
        int ch;
        Channel(const Sample& sample, int channel);
        friend struct Sample;
    };

    Channel operator[](int channel) const;
    const std::byte* operator&() const;

    int number_of_channels() const;
    int channel_size() const;
private:
    // file
    std::shared_ptr<WavFile> file;
    // sample offset
    int32_t sample_offset;
    // bits per sample
    int16_t bits_per_sample;

    Sample(std::shared_ptr<WavFile> f, int32_t off);

    friend class iterator;
};