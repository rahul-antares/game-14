#include <cstdint>
#include <fstream>
#include <cmath>
#include <map>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../../utils/cxxopts.h"
#include "../../utils/log.h"
#include "../../utils/colors.h"
#include "../../utils/exception.h"
#include "../../utils/exception.h"
#include "../../utils/hexdump.h"
#include "../../api/rng.h"

#define MAXCOUNT 1000000
enum OUT_FORMAT { DAT, ASCII_BIN, INT, ALL };

uint64_t parse_size(const std::string& input);
std::string humanReadableSize(size_t bytes);
void dataOut(uint64_t nIters, std::string filePath);
void intOut(uint64_t nIters, bool write, std::string filePath);
void asciiBinOut(uint64_t nIters, bool write, std::string filePath);
void allFormatOut(uint64_t nIters, std::string filePathPrefix="rng");
bool powerOfTwo(int);
std::vector<std::string> formats = {
    "dat",
    "ascii-bin",
    "hex",
    "int",
    "all"
};

std::tuple<uint64_t, std::string, std::string, int, int> parseOptions (int argc, char** argv) {
    uint64_t num;
    std::string fmt = "ascii-bin";
    std::string output = "";

    cxxopts::Options options("bitstream", "A basic C++ driver to generate RNG bit-stream files");
    options.add_options()
        ("n,num",       "Number of random values to generate", cxxopts::value<uint64_t>())
        ("s,size",      "Size of the output file (suffixes = <none>, K, M, G, T)", cxxopts::value<std::string>())
        ("f,format",    "Output format: 'dat', 'ascii-bin', 'hex', 'all'", cxxopts::value<std::string>())
        ("o,output",    "Output file path + name stem (default: stdout)", cxxopts::value<std::string>())
        ("b,row-break", "break row after n characters(power of 2)", cxxopts::value<int>())
        ("w,word-break","break word after n characters(power of 2 and not exceed row-break value)", cxxopts::value<int>())
        ("h,help",      "Print help")
    ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (
        opts.count("num") == 0 &&
        opts.count("size") == 0
    ) {
        std::cout << ANSI_COLOR_RED << "ERROR: at least one of -n or -s options must have a non-zero value\n"<< ANSI_COLOR_RESET << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (
        opts.count("num") != 0 &&
        opts.count("size") != 0
    ) {
        std::cout << ANSI_COLOR_RED << "ERROR: cannot specify -n and -s at the same time\n"<< ANSI_COLOR_RESET << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (opts.count("num") != 0)
        num = opts["num"].as<uint64_t>();
    else {
        auto sizeStr = opts["size"].as<std::string>();
        auto size    = parse_size(sizeStr);
        num = std::ceil(((double)size)/sizeof(uint64_t));
    }

    if (opts.count("format") != 0) {
        fmt = opts["format"].as<std::string>();
        auto it = std::find(formats.begin(), formats.end(), fmt);
        if (it == formats.end()) {
            std::cout << ANSI_COLOR_RED << "ERROR: unsupported out format \"" << fmt << "\""<< ANSI_COLOR_RESET << std::endl;
            std::cout << options.help() << std::endl;
            exit(0);
        }
    }

    if (fmt == "all" && opts.count("output") == 0) {
        std::cout << ANSI_COLOR_RED << "ERROR: format \"all\" requires -o to be specified\n"<< ANSI_COLOR_RESET << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (opts.count("output") != 0)
        output = opts["output"].as<std::string>();

    int rowBreak = -1;
    if (opts.count("row-break") != 0) {
        rowBreak = opts["row-break"].as<int>();

        if (!powerOfTwo(rowBreak)) {
            std::cout << ANSI_COLOR_RED << "ERROR: row-break should be in powers of 2\n"<< ANSI_COLOR_RESET << std::endl;
            std::cout << options.help() << std::endl;
            exit(0);
        }
    }

    int wordBreak = -1;
    if (opts.count("word-break") != 0) {
        wordBreak = opts["word-break"].as<int>();

        if (!powerOfTwo(wordBreak) || wordBreak > rowBreak) {
            std::cout << ANSI_COLOR_RED << "ERROR: word-break should be power of 2 and not exceed row-break value\n"<< ANSI_COLOR_RESET << std::endl;
            std::cout << options.help() << std::endl;
            exit(0);
        }
    }

    return std::tuple{num, fmt, output, rowBreak, wordBreak};
}

void generateBatch(RngLib* r, uint64_t num, std::vector<uint64_t>& buffer) {
    for (uint64_t i = 0; i < num; i++) {
        uint64_t value = r->getRaw();
        buffer.push_back(value);
    }
}

void writeToFile(std::ostringstream& buffer, std::string outFile, std::string format) {
    std::string path;
    std::ofstream file;
    std::ostream* outStream = &std::cout; // Default to std::cout

    if (!outFile.empty()) {
        path = outFile + "." + format;
        file.open(path);
        if (!file)
            throw std::logic_error("Error opening file " + outFile);

        outStream = &file;
    }

    *outStream << buffer.str();
    if (!path.empty()) {
        file.close();
        std::cerr << "written " << humanReadableSize(buffer.str().size()) << " to file " << path << std::endl;
    }
}

void datOut(std::vector<uint64_t>& buffer, std::string outFile){
    std::ostringstream stream;

    for (size_t i = 0; i < buffer.size(); i++)
        stream.write(reinterpret_cast<const char*>(buffer.data() + i), sizeof (uint64_t));

    writeToFile(stream, outFile, "dat");
}

void hexOut(std::vector<uint64_t>& buffer, std::string outFile, int rowBreak, int wordBreak){
    int rowCount  = 0;

    std::ostringstream stream;
    stream << std::hex << std::setfill('0');

    for (size_t i = 0; i < buffer.size(); i++) {
        int wordCount = 0;
        auto value    = buffer[i];
        auto c        = (const unsigned char*)(&value);

        for (size_t i = 0; i < sizeof(value); i++) {
            char tmp[3];
            sprintf (tmp, "%02x", c[i]);
            stream << tmp;

            rowCount++;
            wordCount++;

            if (wordBreak > 0 && wordCount == wordBreak) {
                stream << '\t';
                wordCount = 0;
            }

            if (rowBreak > 0 && rowCount == rowBreak) {
                stream << std::endl;
                rowCount = 0;
                wordCount = 0;
            }
        }
    }

    writeToFile(stream, outFile, "hex");
}

void asciiBinOut(std::vector<uint64_t>& buffer, std::string outFile, int rowBreak){
    int rowCount = 0;
    std::ostringstream stream;

    for (size_t i = 0; i < buffer.size(); i++) {
        auto value = buffer[i];
        auto c = (const unsigned char*)&value;

        for (size_t i = 0; i < sizeof(value); i++) {
            stream << std::bitset<8>(c[i]);
            rowCount++;
            if (rowBreak > 0 && rowCount == rowBreak) {
                stream << std::endl;
                rowCount = 0;
            }
        }
    }

    writeToFile(stream, outFile, "ascii");
}

void dumpOutput (std::vector<uint64_t>& buffer, std::string fmt, std::string outFile, int rowBreak, int wordBreak) {
    if (fmt == "all" || fmt == "ascii-bin")
        asciiBinOut(buffer, outFile, rowBreak);

    if (fmt == "all" || fmt == "dat")
        datOut(buffer, outFile);

    if (fmt == "all" || fmt == "hex")
        hexOut(buffer, outFile, rowBreak, wordBreak);
}

bool powerOfTwo(int n) {
    return (n > 0 ) && !(n & (n-1));
}

int main(int argc, char** argv) {
    std::vector<uint64_t> buffer;

    try {
        auto [iterations, fmt, outFile, rowBreak, wordBreak] = parseOptions(argc, argv);
        auto r = new RngLib();
        r->reseed();

        generateBatch(r, iterations, buffer);
        dumpOutput(buffer, fmt, outFile, rowBreak, wordBreak);
    }
    catch (std::exception& e) {
        fprintf (stderr, "%sError: %s%s\n", ANSI_COLOR_RED, e.what(), ANSI_COLOR_RESET);
    }
    catch (...) {
        fprintf (stderr, "%sError: unknown error%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    }

    return 0;
}

