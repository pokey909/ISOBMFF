#pragma once

#include <XS/PIMPL/Object.hpp>
#include <ISOBMFF/Macros.hpp>
#include <ISOBMFF/FullBox.hpp>
#include <cstdint>

namespace ISOBMFF {
    /**
     * <h1>4cc = "{@value #TYPE}"</h1>
     * aligned(8) class TrackRunBox
     * extends FullBox('trun', version, tr_flags) {
     * unsigned int(32)	sample_count;
     * // the following are optional fields
     * signed int(32)	data_offset;
     * unsigned int(32)	first_sample_flags;
     * // all fields in the following array are optional
     * {
     * unsigned int(32)	sample_duration;
     * unsigned int(32)	sample_size;
     * unsigned int(32)	sample_flags
     * if (version == 0)
     * { unsigned int(32)	sample_composition_time_offset; }
     * else
     * { signed int(32)		sample_composition_time_offset; }
     * }[ sample_count ]
     * }
     */
    class ISOBMFF_EXPORT TRUN : public FullBox, public XS::PIMPL::Object<TRUN> {
    public:
        using XS::PIMPL::Object<TRUN>::impl;


        struct SampleEntry {
            uint32_t sample_duration{0};
            uint32_t sample_size{0};
            uint32_t sample_flags{0};

            uint32_t sample_composition_time_offset{0}; // int32_t for version != 0
        };

        TRUN();

        void ReadData(IParser *parser, BinaryStream &stream) override;

        KeyValueStringList GetDisplayableProperties() const override;

        void WriteDescription(std::ostream &os, std::size_t indentLevel) const override;

        uint32_t GetSampleCount() const;

        // Optional
        uint32_t GetDataOffset() const;
        uint32_t GetFirstSampleFlags() const;
        const std::vector<SampleEntry>& getSampleEntries() const;

        void SetSampleCount(uint32_t value);
        void SetDataOffset(uint32_t value);
        void SetFirstSampleFlags(uint32_t value);
        void AddSampleEntry(const SampleEntry &value);

        bool hasDataOffset() const;
        bool hasFirstSampleFlags() const;
        bool hasSampleDuration() const;
        bool hasSampleSize() const;
        bool hasSampleFlags() const;
        bool hasSampleCTO() const;

    protected:
        size_t getContentSize() const;

    };
}

