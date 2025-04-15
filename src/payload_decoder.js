// function Decoder(bytes, port) {
//     function Decoder(bytes, port) {
//         var decoded = {};
//         decoded.Battery = ((bytes[0] << 8) | bytes[1]);
//         if (bytes[2] === 0) {
//         decoded.Temperature = ((bytes[3] << 8) | bytes[4]);
//         } else {
//         let rawTemp = (bytes[3] << 8) | bytes[4];
//         if (rawTemp & 0x8000) { 
//             rawTemp = rawTemp - 0x10000;
//         }
//         decoded.Temperature = rawTemp;
//       }
//         decoded.Humidity = ((bytes[5] << 8) | bytes[6]);
//         decoded.Velocity = ((bytes[7] << 8) | bytes[8]);
//         return decoded;
//     }
// }

function decodeUplink(input) {
    // extract the raw bytes from the input payload
    var bytes = input.bytes;

    // ensure payload has the expected length (8 bytes for 4 int16 values)
    if (bytes.length !== 8) {
        return {
            warnings: ["payload length is not 6 bytes as expected."],
        };
    }

    // decode the int16 values (big-endian representation)
    var battery = (bytes[0] << 8) | bytes[1];       // first 2 bytes
    var temperature = (bytes[2] << 8) | bytes[3];   // next 2 bytes
    var humidity = (bytes[4] << 8) | bytes[5];      // next 2 bytes
    var velocity = (bytes[6] << 8) | bytes[7];      // last 2 bytes 

    // convert to signed 16-bit integers
    if (battery & 0x8000) battery -= 0x10000;
    if (temperature & 0x8000) temperature -= 0x10000;
    if (humidity & 0x8000) humidity -= 0x10000;
    if (velicity & 0x8000) velocity -= 0x10000;

    // return decoded values as JSON
    return {
        data: {
            Battery: battery,                   // battery level as int16
            Temperature: temperature / 100.0,   // temperature (adjust as needed)
            Humidity: humidity / 100.0,         // humidity (adjust as needed)
            Velocity: velocity                  // velocity level as int16                 
        },
    };
}