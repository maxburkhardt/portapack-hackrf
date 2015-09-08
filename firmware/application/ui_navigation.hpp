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

#include <vector>

namespace ui {

class SystemStatusView : public View {
public:
	SystemStatusView();

private:
	Text portapack {
		{ 0, 0, 15 * 8, 1 * 16 },
		"Max's PortaPack",
	};
};

class NavigationView : public View {
public:
	NavigationView();

	NavigationView(const NavigationView&) = delete;
	NavigationView(NavigationView&&) = delete;

	void push(View* new_view);
	void pop();

	void focus() override;

private:
	std::vector<View*> view_stack;

	Widget* view() const;
	void set_view(Widget* const new_view);
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
