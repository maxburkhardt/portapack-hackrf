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

#ifndef __PROC_NFM_AUDIO_H__
#define __PROC_NFM_AUDIO_H__

#include "baseband_processor.hpp"

#include "dsp_decimate.hpp"
#include "dsp_demodulate.hpp"

#include "audio_output.hpp"
#include "spectrum_collector.hpp"

#include <cstdint>

class NarrowbandFMAudio : public BasebandProcessor {
public:
	void execute(const buffer_c8_t& buffer) override;

	void on_message(const Message* const message) override;

private:
	static constexpr size_t baseband_fs = 3072000;

	std::array<complex16_t, 512> dst;
	const buffer_c16_t dst_buffer {
		dst.data(),
		dst.size()
	};
	std::array<float, 32> audio;
	const buffer_f32_t audio_buffer {
		audio.data(),
		audio.size()
	};

	dsp::decimate::FIRC8xR16x24FS4Decim8 decim_0;
	dsp::decimate::FIRC16xR16x32Decim8 decim_1;
	dsp::decimate::FIRAndDecimateComplex channel_filter;
	uint32_t channel_filter_pass_f = 0;
	uint32_t channel_filter_stop_f = 0;

	dsp::demodulate::FM demod;

	AudioOutput audio_output;

	SpectrumCollector channel_spectrum;

	bool configured { false };
	void configure(const NBFMConfigureMessage& message);
};

#endif/*__PROC_NFM_AUDIO_H__*/
