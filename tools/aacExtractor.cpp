/**
 *
 * Example stream parsing of a fragmented MP4 file
 *
 */

#include <FMP4StreamParser.h>
#include <fstream>
#include <iostream>
#include <ISOBMFF/MFHD.hpp>

void writeToFile(const std::string &filenamePrefix, const std::vector<Frame> &frames) {
    for(int i = 0; i < frames.size(); ++i) {
        std::string filename = "/tmp/aacEx/" + filenamePrefix + "_frame_" + std::to_string(i);
        std::cout << "Writing " << filename << "...\n";
        std::ofstream out(filename);
        out.write((char*)frames[i].data.data(), frames[i].data.size());
        out.close();
    }
}

void fragmentCallback(const Fragment &fragment) {
    auto mfhd = fragment.moof->GetTypedBox<ISOBMFF::MFHD>("mfhd");
    if (mfhd) {
        auto sequenceNumber = mfhd->GetSequenceNumber();
        writeToFile("frag_" + std::to_string(sequenceNumber), fragment.getFrames());
    }
}

int main(int argc, char **argv) {
    FMP4StreamParser parser;
    parser.onFragment(fragmentCallback);


    std::ifstream input("tests/output.m4s");
    if (!input.is_open()) {
        throw "SHIT";
    }

    parser.setInputStream(&input);

    do {
        parser.parse();
    } while(!parser.isEOS());

    return 0;
}

