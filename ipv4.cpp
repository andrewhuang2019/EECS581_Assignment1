// extract_ipv4.cpp
//
// Scans an input line for an IPv4 address (with an optional port) built
// entirely by hand -- no <cstdlib>/<charconv> string-to-number conversion
// functions, no sockets/inet_* address-parsing calls, and no regex.
//
// Grammar (must match EXACTLY within a "candidate run" -- see below):
//
//   address   ::= octet '.' octet '.' octet '.' octet [ ':' port ]
//   octet     ::= 1-3 digits, numeric value 0-255, no leading zero
//                 unless the octet is the single digit "0"
//   port      ::= 1-5 digits, numeric value 0-65535, same no-leading-zero
//                 rule as an octet
//
// Scanning rule:
//   Every character that is not a digit, '.', or ':' is skipped, but a
//   skipped character breaks any digit/period/colon sequence in progress.
//   In other words, the input is chopped into maximal runs made only of
//   the characters '0'-'9', '.', and ':'. Each run, taken as a whole, is
//   then tested against the grammar above. A run must match the grammar
//   *exactly* -- start to end, nothing left over -- to count as a valid
//   address. If a run fails (even if some sub-piece of it would have been
//   a valid address on its own), it is discarded in its entirety and the
//   scan continues with the next run. The first run that fully matches
//   wins, and only one address is ever extracted from a line.
//
//   If a run contains a ':', the port portion must be completely valid
//   (right digit count, in range, no bad leading zero, nothing left over
//   after it) or the ENTIRE run is rejected -- including the address part
//   that preceded the colon.

#include <iostream>
#include <string>

// True for the only characters that may appear inside a candidate run.
static bool isRunChar(char c) {
    return (c >= '0' && c <= '9') || c == '.' || c == ':';
}

// Reads a maximal run of ASCII digits from `run` starting at `pos`.
// Advances `pos` past the digits it consumes and returns them in `digits`.
// Returns false if there were no digits at all at `pos`.
static bool readDigitGroup(const std::string& run, size_t& pos, std::string& digits) {
    size_t start = pos;
    while (pos < run.size() && run[pos] >= '0' && run[pos] <= '9') {
        ++pos;
    }
    digits = run.substr(start, pos - start);
    return !digits.empty();
}

// Manually converts a string of decimal digits into an unsigned long.
// (Plain digit-by-digit accumulation -- not a string-to-number library call.)
static unsigned long digitsToValue(const std::string& digits) {
    unsigned long value = 0;
    for (char c : digits) {
        value = value * 10 + static_cast<unsigned long>(c - '0');
    }
    return value;
}

// Validates a digit group against the shared octet/port rule:
//   - length between minLen and maxLen (inclusive)
//   - no leading zero unless the group is exactly "0"
//   - numeric value <= maxValue
// On success, stores the numeric value in `value` and returns true.
static bool validateDigitGroup(const std::string& digits, size_t maxLen,
                                unsigned long maxValue, unsigned long& value) {
    if (digits.empty() || digits.size() > maxLen) {
        return false;
    }
    if (digits.size() > 1 && digits[0] == '0') {
        return false; // e.g. "01", "00255" are not allowed
    }
    unsigned long v = digitsToValue(digits);
    if (v > maxValue) {
        return false;
    }
    value = v;
    return true;
}

// Attempts to parse `run` (a maximal digit/'.'/':' substring) as EXACTLY
// "octet.octet.octet.octet[:port]" with nothing left over. On success,
// fills outOctets[0..3] and outPort (-1 if no port) and returns true.
static bool tryParseRun(const std::string& run, unsigned long outOctets[4], int& outPort) {
    size_t pos = 0;

    for (int i = 0; i < 4; ++i) {
        std::string digits;
        if (!readDigitGroup(run, pos, digits)) {
            return false; // missing octet (e.g. "..", or a run starting with '.'/':')
        }
        unsigned long value;
        if (!validateDigitGroup(digits, 3, 255, value)) {
            return false;
        }
        outOctets[i] = value;

        if (i < 3) {
            // A '.' must separate each of the first three octets from the next.
            if (pos >= run.size() || run[pos] != '.') {
                return false;
            }
            ++pos; // consume '.'
        }
    }

    // After the 4th octet: either the run ends here (no port), or a ':'
    // introduces a port that must consume the rest of the run exactly.
    if (pos == run.size()) {
        outPort = -1;
        return true;
    }

    if (run[pos] != ':') {
        return false; // leftover junk (e.g. a trailing '.') -- reject the whole run
    }
    ++pos; // consume ':'

    std::string portDigits;
    if (!readDigitGroup(run, pos, portDigits)) {
        return false; // ':' with no port digits following
    }
    unsigned long portValue;
    if (!validateDigitGroup(portDigits, 5, 65535, portValue)) {
        return false; // bad port -> the WHOLE match (address included) is rejected
    }
    if (pos != run.size()) {
        return false; // trailing characters after the port
    }

    outPort = static_cast<int>(portValue);
    return true;
}

// Returns true if a valid address was found, false otherwise.
// On success: outAddress holds the 32-bit value,
// and outPort holds the port number, or -1 if no port was present.
// On failure: outAddress is set to 0 and outPort is set to -1.
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort) {
    outAddress = 0;
    outPort = -1;

    size_t i = 0;
    const size_t n = str.size();

    while (i < n) {
        if (!isRunChar(str[i])) {
            ++i;
            continue;
        }

        // Found the start of a maximal run of digit/'.'/':' characters.
        size_t start = i;
        while (i < n && isRunChar(str[i])) {
            ++i;
        }
        std::string run = str.substr(start, i - start);

        unsigned long octets[4];
        int port;
        if (tryParseRun(run, octets, port)) {
            outAddress = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
            outPort = port;
            return true;
        }
        // This run didn't fully match the grammar -- discard it entirely
        // and keep scanning for the next run (loop continues from `i`).
    }

    return false;
}

int main() {
    std::string line;

    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) {
            break; // EOF on input
        }

        if (line == "END") {
            std::cout << "Program terminated." << std::endl;
            break;
        }

        unsigned long address;
        int port;
        if (extractIPv4(line, address, port)) {
            unsigned long a = (address >> 24) & 0xFF;
            unsigned long b = (address >> 16) & 0xFF;
            unsigned long c = (address >> 8) & 0xFF;
            unsigned long d = address & 0xFF;

            std::cout << "Extracted IPv4 address: "
                      << a << "." << b << "." << c << "." << d
                      << " (decimal value: " << address << ", port: ";
            if (port == -1) {
                std::cout << "none";
            } else {
                std::cout << port;
            }
            std::cout << ")" << std::endl;
        } else {
            std::cout << "Invalid input: no valid IPv4 address found" << std::endl;
        }
    }

    return 0;
}
