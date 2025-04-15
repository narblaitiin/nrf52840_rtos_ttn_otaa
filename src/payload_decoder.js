
function decodeUplink(input) {
    // Input payload is an array of bytes (e.g., input.bytes)
    var bytes = input.bytes;
    var decoded = {};

    // Decode battery voltage (2 bytes, unsigned)
    decoded.Battery = (bytes[0] << 8) | bytes[1]; // Combine bytes

    // Decode temperature (2 bytes, signed)
    decoded.Temperature = (bytes[2] << 8) | bytes[3]; // Combine bytes
    if (decoded.Temperature & 0x8000) {
        decoded.Temperature -= 0x10000; // Convert to signed 16-bit
    }

    // Decode humidity (2 bytes, unsigned)
    decoded.Humidity = (bytes[4] << 8) | bytes[5];

    // Decode ADC voltage (2 bytes, unsigned)
    decoded.Velocity = (bytes[6] << 8) | bytes[7];

    // Return the decoded data
    return {
        data: decoded,
        warnings: [],
        errors: []
    };
}