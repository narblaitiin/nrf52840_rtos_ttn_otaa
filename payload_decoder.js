function decodeUplink(input) {
    // input payload is an array of bytes (e.g., input.bytes)
    var bytes = input.bytes;

    // decode the int32 timestamp (big-endian representation)
    var unixTimestamp = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];

    // convert and display Unix timestamp as a human-readable date and time 
    var date = new Date(unixTimestamp * 1000); // convert seconds to milliseconds
    var readable_date = date.toISOString(); // ISO format (YYYY-MM-DDTHH:mm:ss.sssZ)

    // decode the int16 values (big-endian representation)
    var battery = (bytes[4] << 8) | bytes[5];
    var temperature = (bytes[6] << 8) | bytes[7];
    var humidity = (bytes[8] << 8) | bytes[9];     
    var velocity = (bytes[10] << 8) | bytes[11];

    // convert to signed 16-bit integers
    if (battery & 0x8000) battery -= 0x10000;
    if (temperature & 0x8000) temperature -= 0x10000;
    if (humidity & 0x8000) humidity -= 0x10000;
    if (velocity & 0x8000) velocity -= 0x10000;

    // return decoded values as JSON
    return {
        data: {
            Timestamp: readable_date,               // timestamp (unix format) as int32
            Battery: battery,                   // battery level as int16
            Temperature: temperature / 100.0,   // temperature (adjust as needed)
            Humidity: humidity / 100.0,         // humidity (adjust as needed)
            Velocity: velocity                  // velocity level as int16                 
        },
    };
}