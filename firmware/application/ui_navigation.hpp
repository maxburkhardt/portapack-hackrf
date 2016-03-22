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

#ifndef __UI_NAVIGATION_H__
#define __UI_NAVIGATION_H__

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_focus.hpp"
#include "ui_menu.hpp"

#include "ui_rssi.hpp"
#include "ui_channel.hpp"
#include "ui_audio.hpp"
#include "ui_sd_card_status_view.hpp"

#include <vector>
#include <utility>

namespace ui {

static constexpr uint8_t bitmap_sleep_data[] = {
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x04,
	0x00, 0x08,
	0x00, 0x18,
	0x00, 0x18,
	0x00, 0x38,
	0x00, 0x3c,
	0x00, 0x3c,
	0x00, 0x3e,
	0x84, 0x1f,
	0xf8, 0x1f,
	0xf0, 0x0f,
	0xc0, 0x03,
	0x00, 0x00,
	0x00, 0x00,
};

static constexpr Bitmap bitmap_sleep {
	{ 16, 16 }, bitmap_sleep_data
};

static constexpr uint8_t bitmap_camera_data[] = {
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
	0xcc, 0x03,
	0xe8, 0x07,
	0xfc, 0x3f,
	0x3c, 0x3c,
	0x9c, 0x39,
	0xdc, 0x3b,
	0xdc, 0x3b,
	0x9c, 0x39,
	0x3c, 0x3c,
	0xfc, 0x3f,
	0x00, 0x00,
	0x00, 0x00,
	0x00, 0x00,
};

static constexpr Bitmap bitmap_camera {
	{ 16, 16 }, bitmap_camera_data
};

class SystemStatusView : public View {
public:
	std::function<void(void)> on_back;

	SystemStatusView();

	void set_back_enabled(bool new_value);
	void set_title(const std::string new_value);

private:
	static constexpr auto default_title = "Max's PortaPack";
	static constexpr auto back_text_enabled = " < ";
	static constexpr auto back_text_disabled = " * ";

	Button button_back {
		{ 0 * 8, 0 * 16, 3 * 8, 16 },
		back_text_disabled,
	};

	Text title {
		{ 3 * 8, 0, 16 * 8, 1 * 16 },
		default_title,
	};

	ImageButton button_camera {
		{ 22 * 8, 0, 2 * 8, 1 * 16 },
		&bitmap_camera,
		Color::white(),
		Color::black()
	};

	ImageButton button_sleep {
		{ 25 * 8, 0, 2 * 8, 1 * 16 },
		&bitmap_sleep,
		Color::white(),
		Color::black()
	};

	SDCardStatusView sd_card_status_view {
		{ 28 * 8, 0 * 16,  2 * 8, 1 * 16 }
	};

	void on_camera();
};

class NavigationView : public View {
public:
	std::function<void(const View&)> on_view_changed;

	NavigationView() { }

	NavigationView(const NavigationView&) = delete;
	NavigationView(NavigationView&&) = delete;

	bool is_top() const;

	template<class T, class... Args>
	T* push(Args&&... args) {
		return reinterpret_cast<T*>(push_view(std::unique_ptr<View>(new T(*this, std::forward<Args>(args)...))));
	}

	void pop();

	void focus() override;

private:
	std::vector<std::unique_ptr<View>> view_stack;

	Widget* view() const;

	void free_view();
	void update_view();
	View* push_view(std::unique_ptr<View> new_view);
};

class TranspondersMenuView : public MenuView {
public:
	TranspondersMenuView(NavigationView& nav);
};

class ReceiverMenuView : public MenuView {
public:
	ReceiverMenuView(NavigationView& nav);
};

class SystemMenuView : public MenuView {
public:
	SystemMenuView(NavigationView& nav);
};

class SystemView : public View {
public:
	SystemView(
		Context& context,
		const Rect parent_rect
	);

	Context& context() const override;

private:
	SystemStatusView status_view;
	NavigationView navigation_view;
	Context& context_;
};

class HackRFFirmwareView : public View {
public:
	HackRFFirmwareView(NavigationView& nav);

	void focus() override;

private:
	Text text_title {
		{ 76, 4 * 16, 19 * 8, 16 },
		"HackRF Mode"
	};

	Text text_description_1 {
		{  4, 7 * 16, 19 * 8, 16 },
		"Run stock HackRF firmware and"
	};

	Text text_description_2 {
		{ 12, 8 * 16, 19 * 8, 16 },
		"disable PortaPack until the"
	};

	Text text_description_3 {
		{  4, 9 * 16, 19 * 8, 16 },
		"unit is reset or disconnected"
	};

	Text text_description_4 {
		{ 76, 10 * 16, 19 * 8, 16 },
		"from power?"
	};

	Button button_yes {
		{ 4 * 8, 13 * 16, 8 * 8, 24 },
		"Yes",
	};

	Button button_no {
		{ 18 * 8, 13 * 16, 8 * 8, 24 },
		"No",
	};
};

class NotImplementedView : public View {
public:
	NotImplementedView(NavigationView& nav);

	void focus() override;

private:
	Text text_title {
		{ 5 * 8, 7 * 16, 19 * 8, 16 },
		"Not Yet Implemented"
	};

	Button button_done {
		{ 10 * 8, 13 * 16, 10 * 8, 24 },
		"Bummer",
	};
};

} /* namespace ui */

#endif/*__UI_NAVIGATION_H__*/
