#pragma once

#include <XS/PIMPL/Object.hpp>
#include <ISOBMFF/Macros.hpp>
#include <ISOBMFF/FullBox.hpp>
#include <ISOBMFF/Container.hpp>
#include <cstdint>

namespace ISOBMFF
{
    class ISOBMFF_EXPORT SIDX: public FullBox, public XS::PIMPL::Object< SIDX >
    {
    public:

        struct ReferenceEntry {
            uint32_t reference_type     : 1;
            uint32_t reference_size     : 31;

            uint32_t subsegment_duration;

            uint32_t starts_with_SAP    : 1;
            uint32_t SAP_type           : 3;
            uint32_t SAP_delta_time     : 28;
        };

        using XS::PIMPL::Object< SIDX >::impl;

        SIDX();

        void               ReadData(IParser *parser, BinaryStream &stream) override;
        KeyValueStringList GetDisplayableProperties() const override;
        void WriteDescription( std::ostream & os, std::size_t indentLevel ) const override;

        uint32_t GetReferenceID() const;
        uint32_t GetTimescale() const;
        uint64_t GetEarliestPTS()     const;
        uint64_t GetFirstOffset()     const;

         uint16_t reserved{0};

        uint16_t GetReferenceCount()     const;

        const std::vector<ReferenceEntry>& GetReferenceList() const;

        void SetReferenceID( uint32_t value );
        void SetTimescale( uint32_t value );
        void SetEarliestPTS( uint64_t value );
        void SetFirstOffset( uint64_t value );

        void AddReferenceEntry(const ReferenceEntry& entry);
    };
}

