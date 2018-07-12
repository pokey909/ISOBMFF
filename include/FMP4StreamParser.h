#pragma once

#include <iostream>
#include <vector>
#include "ISOBMFF/IParser.hpp"
#include "ISOBMFF/SIDX.hpp"
#include "ISOBMFF/ContainerBox.hpp"

struct Frame {
    int64_t pts{-1};
    std::vector<uint8_t > data;
};

struct Fragment {
    std::vector<Frame> getFrames() const;
    bool isComplete() const;
    void clear();

    std::shared_ptr<ISOBMFF::SIDX>          sidx;
    std::shared_ptr<ISOBMFF::ContainerBox>  moof;
    std::shared_ptr<ISOBMFF::ContainerBox>  mdat;
};

class FMP4StreamParser {
public:
    using ParsedTopLevelBoxCallback     = std::function<void(const ISOBMFF::Box*)>;
    using SkippedTopLevelBoxCallback    = std::function<void(const std::string&)>;
    using FragmentCallback              = std::function<void(const Fragment&)>;

    FMP4StreamParser();
    ~FMP4StreamParser();

    // Current stream position needs to be at least 8-byte aligned in the mp4 stream.
    void setInputStream(std::istream *input);

    void parse();

    bool isEOS() const;

    void onParsedBox(const std::string  &boxName, const ParsedTopLevelBoxCallback  &callback);
    void onSkippedBox(const std::string &boxName, const SkippedTopLevelBoxCallback &callback);
    void onFragment(const FragmentCallback &fragmentCB);

private:
    struct Private;
    std::unique_ptr<Private> m_impl;
};

