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

#ifndef __PROC_TPMS_H__
#define __PROC_TPMS_H__

#include "baseband_processor.hpp"

#include "channel_decimator.hpp"
#include "matched_filter.hpp"

#include "clock_recovery.hpp"
#include "symbol_coding.hpp"
#include "packet_builder.hpp"

#include "message.hpp"

#include <cstdint>
#include <cstddef>
#include <bitset>

struct NeverMatch {
	bool operator()(const BitHistory&, const size_t) const {
		return false;
	}
};

struct FixedLength {
	bool operator()(const BitHistory&, const size_t symbols_received) const {
		return symbols_received >= length;
	}

	const size_t length;
};

// Translate+rectangular filter
// sample=153.6k, deviation=38400, symbol=19200
// Length: 8 taps, 1 symbols, 2 cycles of sinusoid
constexpr std::array<std::complex<float>, 8> rect_taps_153k6_1t_p { {
	{  1.2500000000e-01f,  0.0000000000e+00f }, {  7.6540424947e-18f,  1.2500000000e-01f },
	{ -1.2500000000e-01f,  1.5308084989e-17f }, { -2.2962127484e-17f, -1.2500000000e-01f },
	{  1.2500000000e-01f, -3.0616169979e-17f }, {  3.8270212473e-17f,  1.2500000000e-01f },
	{ -1.2500000000e-01f,  4.5924254968e-17f }, { -5.3578297463e-17f, -1.2500000000e-01f },
} };

class TPMSProcessor : public BasebandProcessor {
public:
	using payload_t = std::bitset<1024>;

	void execute(buffer_c8_t buffer) override;

private:
	ChannelDecimator decimator { ChannelDecimator::DecimationFactor::By16 };
	dsp::matched_filter::MatchedFilter mf { rect_taps_153k6_1t_p, 4 };

	clock_recovery::ClockRecovery<clock_recovery::FixedErrorFilter> clock_recovery {
		38400, 19200, { 0.0555f },
		[this](const float symbol) { this->consume_symbol(symbol); }
	};
	PacketBuilder<BitPattern, NeverMatch, FixedLength> packet_builder {
		{ 0b010101010101010101010101010110, 30, 1 },
		{ },
		{ 256 },
		[this](const payload_t& payload, const size_t bits_received) {
			this->payload_handler(payload, bits_received);
		}
	};

	void consume_symbol(const float symbol);
	void payload_handler(const payload_t& payload, const size_t bits_received);
};

#endif/*__PROC_TPMS_H__*/
