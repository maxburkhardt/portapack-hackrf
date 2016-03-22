/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "manchester.hpp"

#include "string_format.hpp"

ManchesterDecoder::DecodedSymbol ManchesterDecoder::operator[](const size_t index) const {
	const size_t encoded_index = index * 2;
	if( (encoded_index + 1) < packet.size() ) {
		const auto value = packet[encoded_index + sense];
		const auto error = packet[encoded_index + 0] == packet[encoded_index + 1];
		return { value, error };
	} else {
		return { 0, 1 };
	}
}

size_t ManchesterDecoder::symbols_count() const {
	return packet.size() / 2;
}

ManchesterFormatted format_manchester(
	const ManchesterDecoder& decoder
) {
	const size_t payload_length_decoded = decoder.symbols_count();
	const size_t payload_length_hex_characters = (payload_length_decoded + 3) / 4;
	const size_t payload_length_symbols_rounded = payload_length_hex_characters * 4;

	std::string hex_data;
	std::string hex_error;
	hex_data.reserve(payload_length_hex_characters);
	hex_error.reserve(payload_length_hex_characters);

	uint_fast8_t data = 0;
	uint_fast8_t error = 0;
	for(size_t i=0; i<payload_length_symbols_rounded; i++) {
		const auto symbol = decoder[i];

		data <<= 1;
		data |= symbol.value;

		error <<= 1;
		error |= symbol.error;

		if( (i & 3) == 3 ) {
			hex_data += to_string_hex(data & 0xf, 1);
			hex_error += to_string_hex(error & 0xf, 1);
		}
	}

	return { hex_data, hex_error };
}
