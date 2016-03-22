/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
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

#ifndef __AUDIO_OUTPUT_H__
#define __AUDIO_OUTPUT_H__

#include "dsp_types.hpp"

#include "dsp_iir.hpp"
#include "dsp_squelch.hpp"

#include "block_decimator.hpp"
#include "audio_stats_collector.hpp"

#include <cstdint>

class AudioOutput {
public:
	void configure(
		const iir_biquad_config_t& hpf_config,
		const iir_biquad_config_t& deemph_config = iir_config_passthrough,
		const float squelch_threshold = 0.0f
	);

	void write(const buffer_s16_t& audio);
	void write(const buffer_f32_t& audio);

private:
	static constexpr float k = 32768.0f;
	static constexpr float ki = 1.0f / k;

	BlockDecimator<float, 32> block_buffer { 1 };	

	IIRBiquadFilter hpf;
	IIRBiquadFilter deemph;
	FMSquelch squelch;

	AudioStatsCollector audio_stats;

	uint64_t audio_present_history = 0;

	void on_block(const buffer_f32_t& audio);
	void fill_audio_buffer(const buffer_f32_t& audio);
	void feed_audio_stats(const buffer_f32_t& audio);
};

#endif/*__AUDIO_OUTPUT_H__*/
