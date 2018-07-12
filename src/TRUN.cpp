#include <ISOBMFF/TRUN.hpp>
#include <ISOBMFF/ContainerBox.hpp>

template<>
class XS::PIMPL::Object< ISOBMFF::TRUN >::IMPL
{
public:

    IMPL()                  = default;
    IMPL( const IMPL & o )  = default;
    ~IMPL()                 = default;

    uint32_t sample_count{0};
    int32_t  data_offset{0};
    uint32_t first_sample_flags{0};

    std::vector<ISOBMFF::TRUN::SampleEntry> entries;
};

#define XS_PIMPL_CLASS ISOBMFF::TRUN
#include <XS/PIMPL/Object-IMPL.hpp>

namespace ISOBMFF{


    std::string SampleEntryToString(const TRUN::SampleEntry &entry, int version) {
        std::string out = "[";
        out += "Sample_duration: " + std::to_string(entry.sample_duration) + "   ";
        out += "Sample_size: " + std::to_string(entry.sample_size) + "   ";
        out += "Sample_flags: " + std::to_string(entry.sample_flags) + "   ";
        if (version == 0) {
            out += "sample_cto: " + std::to_string((int32_t)entry.sample_composition_time_offset) + "   ";
        } else {
            out += "sample_cto: " + std::to_string(entry.sample_composition_time_offset) + "   ";
        }
        out += "]";
        return out;
    }

    TRUN::TRUN() : FullBox("trun") {

    }

    void TRUN::ReadData(IParser *parser, BinaryStream &stream) {
        FullBox::ReadData(parser, stream);
        SetSampleCount( stream.ReadBigEndianUInt32() );
        if (hasDataOffset()) {
            impl->data_offset = stream.ReadBigEndianUInt32();
        }
        if (hasFirstSampleFlags()) {
            impl->first_sample_flags = stream.ReadBigEndianUInt32();
        }
        for(int i = 0; i < impl->sample_count; ++i) {
            SampleEntry entry;
            if (hasSampleDuration()) {
                entry.sample_duration = stream.ReadBigEndianUInt32();
            }
            if (hasSampleSize()) {
                entry.sample_size = stream.ReadBigEndianUInt32();
            }
            if (hasSampleFlags()) {
                entry.sample_flags = stream.ReadBigEndianUInt32();
            }
            if (hasSampleCTO()) {
                entry.sample_composition_time_offset = stream.ReadBigEndianUInt32();
            }
            AddSampleEntry(entry);
        }
    }

    KeyValueStringList TRUN::GetDisplayableProperties() const {
        auto props = FullBox::GetDisplayableProperties();
        props.push_back( { "SampleCount", std::to_string( GetSampleCount() ) } );
        props.push_back( { "DataOffset", std::to_string( GetDataOffset() ) } );
        props.push_back( { "FirstSampleFlags", std::to_string( GetFirstSampleFlags() ) } );

        for(int i = 0; i < impl->entries.size(); ++i) {
            props.push_back({"SampleEntry[" + std::to_string(i) + "]",
                             SampleEntryToString(impl->entries[i], GetVersion()) });
        }
        return props;
    }

    void TRUN::WriteDescription(std::ostream &os, std::size_t indentLevel) const {
        FullBox::WriteDescription( os, indentLevel );
    }

    uint32_t TRUN::GetSampleCount() const {
        return impl->sample_count;
    }

    uint32_t TRUN::GetDataOffset() const {
        return impl->data_offset;
    }

    uint32_t TRUN::GetFirstSampleFlags() const {
        return impl->first_sample_flags;
    }

    const std::vector<TRUN::SampleEntry> &TRUN::getSampleEntries() const {
        return impl->entries;
    }

    void TRUN::SetSampleCount(uint32_t value) {
        impl->sample_count = value;
    }

    void TRUN::SetDataOffset(uint32_t value) {
        impl->data_offset = value;
    }

    void TRUN::SetFirstSampleFlags(uint32_t value) {
        impl->first_sample_flags = value;
    }

    void TRUN::AddSampleEntry(const TRUN::SampleEntry &value) {
        impl->entries.push_back(value);
    }

    size_t TRUN::getContentSize() const {
        auto flags = GetFlags();
        uint32_t contentSize = 8;

        if(hasDataOffset()) {
            contentSize += 4;
        }
        if(hasFirstSampleFlags()) {
            contentSize += 4;
        }

        uint32_t entrySize = 0;
        if(hasSampleDuration()) {
            entrySize += 4;
        }
        if(hasSampleSize()) {
            entrySize += 4;
        }
        if(hasSampleFlags()) {
            entrySize += 4;
        }
        if(hasSampleCTO()) {
            entrySize += 4;
        }

        return contentSize + entrySize * impl->entries.size();
    }

    bool TRUN::hasDataOffset() const {
        return (GetFlags() & 0x1u) != 0;
    }

    bool TRUN::hasFirstSampleFlags() const {
        return (GetFlags() & 0x4u) != 0;
    }

    bool TRUN::hasSampleDuration() const {
        return (GetFlags() & 0x100u) != 0;
    }

    bool TRUN::hasSampleSize() const {
        return (GetFlags() & 0x200u) != 0;
    }

    bool TRUN::hasSampleFlags() const {
        return (GetFlags() & 0x400u) != 0;
    }

    bool TRUN::hasSampleCTO() const {
        return (GetFlags() & 0x800u) != 0;
    }

}
