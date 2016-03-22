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

#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include <string>

#include "file.hpp"
#include "sd_card.hpp"

#include "lpc43xx_cpp.hpp"
using namespace lpc43xx;

class LogFile {
public:
	LogFile(const std::string& file_path);
	~LogFile();

	bool is_ready();

	bool write_entry(const rtc::RTC& datetime, const std::string& entry);

private:
	const std::string file_path;
	
	File file;

	SignalToken sd_card_status_signal_token;

	bool write(const std::string& message);

	void on_sd_card_status(const sd_card::Status status);
};

#endif/*__LOG_FILE_H__*/
