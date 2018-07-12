#include <ISOBMFF/SIDX.hpp>
#include <ISOBMFF/ContainerBox.hpp>

template<>
class XS::PIMPL::Object< ISOBMFF::SIDX >::IMPL
{
public:

    IMPL()                  = default;
    IMPL( const IMPL & o )  = default;
    ~IMPL()                 = default;

    uint32_t reference_id{0};
    uint32_t timescale{0};

    uint64_t earliest_PTS{0}; // 32bit for version 0
    uint64_t first_offset{0}; // 32bit for version 0

    uint16_t reserved{0};
    uint16_t reference_count{0};

    std::vector<ISOBMFF::SIDX::ReferenceEntry> reference_entries;
};

#define XS_PIMPL_CLASS ISOBMFF::SIDX
#include <XS/PIMPL/Object-IMPL.hpp>

namespace ISOBMFF{

    std::string RefernceEntryToString(const SIDX::ReferenceEntry &entry) {
        std::string out = "[";
        out += "Reference_type: " + std::to_string(entry.reference_type) + "   ";
        out += "Reference_size: " + std::to_string(entry.reference_size) + "   ";
        out += "SAP_delta_time: " + std::to_string(entry.SAP_delta_time) + "   ";
        out += "SAP_type: " + std::to_string(entry.SAP_type) + "   ";
        out += "Starts_with_SAP: " + std::to_string(entry.starts_with_SAP) + "   ";
        out += "Subsegment_duration: " + std::to_string(entry.subsegment_duration);
        out += "]";
        return out;
    }
    SIDX::SIDX() : FullBox("sidx") {

    }

    void SIDX::ReadData(IParser *parser, BinaryStream &stream) {
        FullBox::ReadData(parser, stream);

        SetReferenceID( stream.ReadBigEndianUInt32() );
        SetTimescale  ( stream.ReadBigEndianUInt32() );

        if( GetVersion() == 1 ) {
            SetEarliestPTS( stream.ReadBigEndianUInt64() );
            SetFirstOffset( stream.ReadBigEndianUInt64() );
        }
        else {
            SetEarliestPTS( stream.ReadBigEndianUInt32() );
            SetFirstOffset( stream.ReadBigEndianUInt32() );
        }

        impl->reserved = stream.ReadBigEndianUInt16();
        impl->reference_count = stream.ReadBigEndianUInt16();

        for(int i = 0; i < impl->reference_count; ++i) {
            impl->reference_entries.emplace_back(ReferenceEntry());
            ReferenceEntry& entry = impl->reference_entries.back();

            uint32_t tmp = stream.ReadBigEndianUInt32();
            entry.reference_type = tmp & (uint32_t)1;
            entry.reference_size = tmp & ~(uint32_t)1;
            entry.subsegment_duration = stream.ReadBigEndianUInt32();

            tmp = stream.ReadBigEndianUInt32();
            entry.starts_with_SAP = tmp & (uint32_t)1;
            entry.SAP_type = tmp & ((uint32_t)0x7 << 1u);
            entry.SAP_delta_time = tmp & ((uint32_t)0x15 >> 4u);
        }
    }

    KeyValueStringList SIDX::GetDisplayableProperties() const {
        auto props = FullBox::GetDisplayableProperties();
        props.push_back( { "Reference_ID", std::to_string( GetReferenceID() ) } );
        props.push_back( { "Timescale", std::to_string( GetTimescale() ) } );
        props.push_back( { "Earliest_PTS", std::to_string( GetEarliestPTS() ) } );
        props.push_back( { "First_Offset", std::to_string( GetFirstOffset() ) } );
        for(int i = 0; i < GetReferenceCount(); ++i) {
            props.push_back( { "Reference[" + std::to_string(i) + "]", RefernceEntryToString(impl->reference_entries[i])} );
        }
        return props;
    }

    uint32_t SIDX::GetReferenceID() const {
        return impl->reference_id;
    }

    uint32_t SIDX::GetTimescale() const {
        return impl->timescale;
    }

    uint64_t SIDX::GetEarliestPTS() const {
        return impl->earliest_PTS;
    }

    uint64_t SIDX::GetFirstOffset() const {
        return impl->first_offset;
    }

    uint16_t SIDX::GetReferenceCount() const {
        return impl->reference_count;
    }

    const std::vector<SIDX::ReferenceEntry> &SIDX::GetReferenceList() const {
        return impl->reference_entries;
    }

    void SIDX::SetReferenceID(uint32_t value) {
        impl->reference_id = value;
    }

    void SIDX::SetTimescale(uint32_t value) {
        impl->timescale = value;
    }

    void SIDX::SetEarliestPTS(uint64_t value) {
        impl->earliest_PTS = value;
    }

    void SIDX::SetFirstOffset(uint64_t value) {
        impl->first_offset =value;
    }

    void SIDX::AddReferenceEntry(const SIDX::ReferenceEntry &entry) {
        impl->reference_entries.push_back(entry);
    }

    void SIDX::WriteDescription(std::ostream &os, std::size_t indentLevel) const {
        FullBox::WriteDescription( os, indentLevel );
    }
}
