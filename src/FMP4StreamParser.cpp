#include "FMP4StreamParser.h"
#include "ISOBMFF/Boxes.h"
#include <string>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <deque>
#include <array>
#include <ISOBMFF/File.hpp>
#include <fstream>
#include <unordered_map>


bool Fragment::isComplete() const {
    return sidx && moof && mdat;
}

void Fragment::clear() {
    sidx.reset();
    moof.reset();
    mdat.reset();
}

void AddFragmentBox(Fragment &frag, const std::shared_ptr<ISOBMFF::Box> &box) {
    auto name = box->GetName();
    if(name == "moof") {
        frag.moof = std::static_pointer_cast<ISOBMFF::ContainerBox>(box);
    } else if (name == "sidx") {
        frag.sidx = std::static_pointer_cast<ISOBMFF::SIDX>(box);
    } else if (name == "mdat") {
        frag.mdat = std::static_pointer_cast<ISOBMFF::ContainerBox>(box);
    }
}

std::vector<Frame> Fragment::getFrames() const {
    auto traf = (ISOBMFF::ContainerBox*)moof->GetBox("traf").get();
    auto tfhd = (ISOBMFF::TFHD*)traf->GetBox("tfhd").get();
    auto trun = (ISOBMFF::TRUN*)traf->GetBox("trun").get();
    auto data = mdat->GetData();

    size_t ofs = 0;
    uint64_t curPts = sidx->GetEarliestPTS();
    std::vector<Frame> frames;
    frames.reserve(trun->GetSampleCount());
    for(const auto &sample : trun->getSampleEntries()) {
        Frame frame;
        frame.data = { data.begin() + ofs, data.begin() + ofs + sample.sample_size };
        frame.pts = curPts ;
        frames.emplace_back(frame);
        curPts += tfhd->GetDefaultSampleDuration();
        ofs += sample.sample_size;
    }
    return frames;
}


struct FMP4StreamParser::Private : public ISOBMFF::IParser {
    static constexpr size_t DefaultBufferSize = 1024 * 70; // preallocate 20kb.
    enum ParseState : uint8_t {
        Parse_SIDX = 0u,
        Parse_MOOF,
        Parse_MDAT,
        Parse_Done
    };

    Private() {
        m_inBuffer.reserve(DefaultBufferSize);
        registerBoxes();
        m_root = std::make_shared<ISOBMFF::File>();
    }

    ~Private() override {
        m_input = nullptr;
    }

    void setInputStream(std::istream *input) {
        m_input = input;
    }

    void readAll() {
        continueParsing();
    }

    /*
     *
     * IParser implementation
     *
     */

    std::shared_ptr<ISOBMFF::Box> CreateBox(const std::string &type) const override {
        auto registeredBox = m_registeredBoxes.find(type);
        if (registeredBox != m_registeredBoxes.end()) {
            return registeredBox->second();
        }
        return std::make_shared<ISOBMFF::Box>( type );
    }

    ISOBMFF::IParser::StringType GetPreferredStringType() const override {
        return ISOBMFF::IParser::StringType::NULLTerminated;
    }

    void SetPreferredStringType(ISOBMFF::IParser::StringType value) override{

    }

    uint64_t GetOptions() const override {
        return m_options;
    }

    void SetOptions(uint64_t value) override {
        m_options = value;
    }

    void AddOption(ISOBMFF::IParser::Options option) override {

    }

    void RemoveOption(ISOBMFF::IParser::Options option) override {

    }

    bool HasOption(ISOBMFF::IParser::Options option) override {
        return (m_options & static_cast<uint64_t>(option)) != 0;
    }

    const void* GetInfo(const std::string &key) override {
        return nullptr;
    }

    void SetInfo(const std::string &key, void *value) override {

    }

    bool eos() const {
        return m_input && m_input->eof();
    }

    void onParsedBox(const std::string &boxName, const FMP4StreamParser::ParsedTopLevelBoxCallback &callback) {
        m_callbacks.insert(std::make_pair(boxName, callback));
    }

    void onSkippedBox(const std::string &boxName, const FMP4StreamParser::SkippedTopLevelBoxCallback &callback) {
        m_callbacksSkippedBox.insert(std::make_pair(boxName, callback));
    }

    void onFragment(const FragmentCallback &fragmentCB) {
        m_fragmentCallback = fragmentCB;
    }

private:
    using BoxFactoryFunc = std::function<std::shared_ptr<ISOBMFF::Box>(void)>;

    std::istream*           m_input{nullptr};
    Fragment                m_currentFramgment;
    std::deque<Frame>       m_frames;
    std::vector<uint8_t>    m_inBuffer;
    ParseState              m_state{ParseState::Parse_SIDX};
    uint64_t                m_options{IParser::Options::ShowBoxContentDebug | IParser::Options::SkipNotRequiredBoxes};
    size_t                  m_writeIndex{0};
    std::unordered_map<std::string, BoxFactoryFunc> m_registeredBoxes;
    std::unordered_map< std::string, void * >       m_info;
    std::shared_ptr<ISOBMFF::File>        m_root;
    std::unordered_map<std::string, FMP4StreamParser::ParsedTopLevelBoxCallback>    m_callbacks;
    std::unordered_map<std::string, FMP4StreamParser::SkippedTopLevelBoxCallback>   m_callbacksSkippedBox;
    FMP4StreamParser::FragmentCallback                                              m_fragmentCallback;

    struct BoxHeader {
        uint32_t boxSize    {0};
        char     boxId[4]   {0};
    };

    void notifyParsedBox(const ISOBMFF::Box *box) {
        const auto &callbackPair = m_callbacks.find(box->GetName());
        if (callbackPair != m_callbacks.end()) {
            callbackPair->second(box);
        }
    }
    void notifySkippeddBox(const BoxHeader &header) {
        std::string name(header.boxId, 4);
        const auto &callbackPair = m_callbacksSkippedBox.find(name);
        if (callbackPair != m_callbacksSkippedBox.end()) {
            callbackPair->second(name);
        }
    }

    void notifyFragment(const Fragment &frag) {
        if (m_fragmentCallback) {
            m_fragmentCallback(frag);
        }
    }

    bool readBox() {
        static const std::vector<std::string> requiredBoxes = { "ftyp", "sidx", "moof", "mdat" };

        BoxHeader header;
        for(auto &boxId : requiredBoxes) {
            if (canReadBox(boxId, header)) {
                std::vector<uint8_t> boxData = {m_inBuffer.begin(), m_inBuffer.begin() + header.boxSize};
                std::string name(header.boxId, 4);
                if (name == "mdat") {
                    int i = 0;
                }
                ISOBMFF::BinaryStream boxStream(boxData);
                auto box = CreateBox(name);
                m_root->ReadData(this, boxStream);
                AddFragmentBox(m_currentFramgment, m_root->GetBoxes().back());
                m_inBuffer.erase(m_inBuffer.begin(), m_inBuffer.begin() + header.boxSize);
                notifyParsedBox(m_root->GetBoxes().back().get());
                return true;
            }
        }
        ;
        return skipUnkownBox(header) && !m_inBuffer.empty();
    }

    // Returns false if not enough bytes available to skip the full box
    bool skipUnkownBox(const BoxHeader &header) {
        if (m_inBuffer.size() > header.boxSize) {
            m_inBuffer.erase(m_inBuffer.begin(), m_inBuffer.begin() + header.boxSize);
            notifySkippeddBox(header);
            return true;
        }
        return false;
    }

    bool canReadBox(const std::string &boxId, BoxHeader &header) {
        return isBox(boxId, header) && header.boxSize <= m_inBuffer.size();
    }

    void extractFrames(const Fragment &fragment) {
        if (fragment.isComplete()) {
            notifyFragment(fragment);
            m_currentFramgment.clear();
        }
    }

    void continueParsing() {
        BoxHeader header;
        auto bytesRead = readInputStream();
        while(readBox()) {
            extractFrames(m_currentFramgment);
        }
    }

    // Reads until input stream has no more data currently available or is EOF
    std::streamsize readInputStream() {
        static char readBuf[8192];
        size_t totalBytesRead = 0;
        std::streamsize bytesRead = 0;
        do {
            m_input->read(readBuf, sizeof(readBuf));
            bytesRead = m_input->gcount();
            totalBytesRead += bytesRead;
            m_inBuffer.insert(m_inBuffer.end(), (uint8_t *)readBuf, (uint8_t *)readBuf + bytesRead);
        } while (bytesRead == sizeof(readBuf));
        return totalBytesRead;
    }

    bool isBox(const std::string &boxId, BoxHeader &header) const {
        if (m_inBuffer.size() < sizeof(BoxHeader)) {
            return false;
        }
        size_t bsizs = sizeof(BoxHeader);
        std::vector<uint8_t> boxSyncWord(m_inBuffer.begin(), m_inBuffer.begin() + sizeof(BoxHeader));
        ISOBMFF::BinaryStream bstream(boxSyncWord);
        header.boxSize = bstream.ReadBigEndianUInt32();
        std::copy_n(m_inBuffer.data() + sizeof(uint32_t), sizeof(header.boxId), header.boxId);
        return std::string(header.boxId, sizeof(header.boxId)) == boxId;
    }

    template<class BoxType = ISOBMFF::ContainerBox>
    void createBoxType(std::shared_ptr<BoxType> &instance, const std::string &name) {
        instance = std::make_shared<BoxType>();
    }
    template<class BoxType = ISOBMFF::ContainerBox>
    void registerBox( const std::string & type ) {
        if( type.size() != 4 ) {
            throw std::runtime_error( "Box name should be 4 characters long" );
        }
        m_registeredBoxes.insert( std::make_pair(type, [this, type]() -> std::shared_ptr< ISOBMFF::Box > {
            std::shared_ptr<BoxType> inst;
            createBoxType(inst, type);
            return std::static_pointer_cast<ISOBMFF::Box>(inst);
        }) );
    }


    void registerBoxes() {
        using namespace ISOBMFF;
        
        // Boxes with data
        registerBox<FTYP>( "ftyp" );
        registerBox<MVHD>( "mvhd" );
        registerBox<MFHD>( "mfhd" );
        registerBox<TKHD>( "tkhd" );
        registerBox<META>( "meta" );
        registerBox<HDLR>( "hdlr" );
        registerBox<PITM>( "pitm" );
        registerBox<IINF>( "iinf" );
        registerBox<DREF>( "dref" );
        registerBox<URL>( "url " );
        registerBox<URN>( "urn " );
        registerBox<ILOC>( "iloc" );
        registerBox<IREF>( "iref" );
        registerBox<INFE>( "infe" );
        registerBox<IROT>( "irot" );
        registerBox<HVCC>( "hvcC" );
        registerBox<DIMG>( "dimg" );
        registerBox<THMB>( "thmb" );
        registerBox<CDSC>( "cdsc" );
        registerBox<COLR>( "colr" );
        registerBox<ISPE>( "ispe" );
        registerBox<IPMA>( "ipma" );
        registerBox<PIXI>( "pixi" );
        registerBox<IPCO>( "ipco" );
        registerBox<STSD>( "stsd" );
        registerBox<SIDX>( "sidx" );
        registerBox<FRMA>( "frma" );
        registerBox<SCHM>( "schm" );
        registerBox<TRUN>( "trun" );
        registerBox<TFHD>( "tfhd" );

        // Container boxes
        registerBox( "moov" );
        registerBox( "trak" );
        registerBox( "edts" );
        registerBox( "mdia" );
        registerBox( "minf" );
        registerBox( "stbl" );
        registerBox( "mvex" );
        registerBox( "moof" );
        registerBox( "traf" );
        registerBox( "mfra" );
        registerBox( "skip" );
        registerBox( "meco" );
        registerBox( "mere" );
        registerBox( "dinf" );
        registerBox( "ipro" );
        registerBox( "sinf" );
        registerBox( "iprp" );
        registerBox( "fiin" );
        registerBox( "paen" );
        registerBox( "strk" );
        registerBox( "tapt" );
        registerBox( "schi" );

    }
};

template<>
void FMP4StreamParser::Private::createBoxType(std::shared_ptr<ISOBMFF::ContainerBox> &instance, const std::string &name) {
    instance = std::make_shared<ISOBMFF::ContainerBox>(name);
}
template<>
void FMP4StreamParser::Private::createBoxType(std::shared_ptr<ISOBMFF::Box> &instance, const std::string &name) {
    instance = std::make_shared<ISOBMFF::Box>(name);
}


FMP4StreamParser::FMP4StreamParser() {
    m_impl = std::make_unique<Private>();
}

FMP4StreamParser::~FMP4StreamParser() = default;

void FMP4StreamParser::setInputStream(std::istream *input) {
    m_impl->setInputStream(input);
}

void FMP4StreamParser::parse() {
    return m_impl->readAll();
}

bool FMP4StreamParser::isEOS() const {
    return m_impl->eos();
}

void FMP4StreamParser::onParsedBox(const std::string &boxName, const FMP4StreamParser::ParsedTopLevelBoxCallback &callback) {
    m_impl->onParsedBox(boxName, callback);
}

void FMP4StreamParser::onSkippedBox(const std::string &boxName, const FMP4StreamParser::SkippedTopLevelBoxCallback &callback) {
    m_impl->onSkippedBox(boxName, callback);
}

void FMP4StreamParser::onFragment(const FragmentCallback &fragmentCB) {
    m_impl->onFragment(fragmentCB);
}


