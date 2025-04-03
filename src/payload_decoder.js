/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

function Decoder(bytes, port) {
    function Decoder(bytes, port) {
        var decoded = {};
        decoded.Battery = ((bytes[0] << 8) | bytes[1]);
        if (bytes[2] === 0) {
        decoded.Temperature = ((bytes[3] << 8) | bytes[4]);
        } else {
        let rawTemp = (bytes[3] << 8) | bytes[4];
        if (rawTemp & 0x8000) { 
            rawTemp = rawTemp - 0x10000;
        }
        decoded.Temperature = rawTemp;
      }
        decoded.Humidity = ((bytes[5] << 8) | bytes[6]);
        decoded.Velocity = ((bytes[7] << 8) | bytes[8]);
        return decoded;
    }
}