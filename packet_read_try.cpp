#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>

// Example: define your packet header/layout here.
// Suppose each packet in the .nbin file looks like:
// [uint32_t packet_length][uint8_t  type][payload_bytes...]
struct PacketHeader {
    uint32_t packet_length;
    uint8_t  type;
    // (other header fields...)
};

std::vector<uint8_t> readPacket(const std::string& filename,
                                std::streamoff packetOffset)
{
    // Open the file in binary mode
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Seek to the start of the packet
    in.seekg(packetOffset, std::ios::beg);
    if (!in) {
        throw std::runtime_error("Failed to seek to offset: " + std::to_string(packetOffset));
    }

    // Read the fixed-size header first
    PacketHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in) {
        throw std::runtime_error("Failed to read packet header.");
    }

    std::cout << "Packet length: " << hdr.packet_length
              << ", Type: " << static_cast<int>(hdr.type) << "\n";

    // Compute how many payload bytes remain
    size_t payloadSize = hdr.packet_length - sizeof(hdr);
    if (payloadSize == 0) {
        return {};  // no payload
    }

    // Read the payload
    std::vector<uint8_t> payload(payloadSize);
    in.read(reinterpret_cast<char*>(payload.data()), payloadSize);
    if (!in) {
        throw std::runtime_error("Failed to read packet payload.");
    }

    return payload;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file.nbin> <packet_offset>\n";
        return 1;
    }

    const std::string filename  = argv[1];
    const std::streamoff offset = std::stoll(argv[2]);

    try {
        auto payload = readPacket(filename, offset);
        std::cout << "Read payload of " << payload.size() << " bytes.\n";

        // Example: dump first few bytes in hex
        for (size_t i = 0; i < std::min(payload.size(), size_t(16)); ++i) {
            printf("%02X ", payload[i]);
        }
        std::cout << "\n";

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
