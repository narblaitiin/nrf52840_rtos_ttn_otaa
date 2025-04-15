function decodeUplink(input) {
    // input payload is an array of bytes (e.g., input.bytes)
    var bytes = input.bytes;
    var decoded = {};

    // decode battery voltage (2 bytes, unsigned)
    decoded.Battery = (bytes[0] << 8) | bytes[1]; // Combine bytes

    // decode temperature (2 bytes, signed)
    decoded.Temperature = (bytes[2] << 8) | bytes[3]; // combine bytes
    if (decoded.Temperature & 0x8000) {
        decoded.Temperature -= 0x10000; // convert to signed 16-bit
    }

    // decode humidity (2 bytes, unsigned)
    decoded.Humidity = (bytes[4] << 8) | bytes[5];

    // decode ADC voltage (2 bytes, unsigned)
    decoded.Velocity = (bytes[6] << 8) | bytes[7];

    // return the decoded data
    return {
        data: decoded,
        warnings: [],
        errors: []
    };
}