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

#ifndef __UI_WIDGET_H__
#define __UI_WIDGET_H__

#include "ui.hpp"
#include "ui_text.hpp"
#include "ui_painter.hpp"
#include "ui_focus.hpp"

#include "utility.hpp"

#include "message.hpp"

#include <memory>
#include <vector>
#include <string>

namespace ui {

void dirty_set();
void dirty_clear();
bool is_dirty();

// TODO: Move these somewhere else!
// TODO: Allow l=0 to not fill/justify? Already using this way in ui_spectrum.hpp...
std::string to_string_dec_uint(const uint32_t n, const int32_t l = 0, const char fill = 0);
std::string to_string_dec_int(const int32_t n, const int32_t l = 0, const char fill = 0);
std::string to_string_hex(const uint32_t n, const int32_t l = 0);

class Context {
public:
	FocusManager& focus_manager() {
		return focus_manager_;
	}

	MessageHandlerMap& message_map() {
		return message_map_;
	}

private:
	FocusManager focus_manager_;
	MessageHandlerMap message_map_;
};

class Widget {
public:
	constexpr Widget(
	) : parent_rect { }
	{
	}

	constexpr Widget(
		Rect parent_rect
	) : parent_rect { parent_rect }
	{
	}

	Widget(const Widget&) = delete;
	Widget(Widget&&) = delete;

	virtual ~Widget() = default;

	Point screen_pos();
	Size size() const;
	Rect screen_rect();
	virtual void set_parent_rect(const Rect new_parent_rect);

	Widget* parent() const;
	void set_parent(Widget* const widget);

	bool hidden() const { return flags.hidden; }
	void hidden(bool hide);

	virtual void focus();
	virtual void on_focus();
	virtual void blur();
	virtual void on_blur();
	bool focusable() const;
	bool has_focus();
	virtual Widget* last_child_focus() const;
	virtual void set_last_child_focus(Widget* const child);

	virtual void paint(Painter& painter) = 0;

	virtual void on_show() { };
	virtual void on_hide() { };

	virtual bool on_key(const KeyEvent event);
	virtual bool on_encoder(const EncoderEvent event);
	virtual bool on_touch(const TouchEvent event);
	virtual const std::vector<Widget*> children() const;

	virtual Context& context() const;

	void set_style(const Style* new_style);

	const Style& style() const;

	// State management methods.
	void set_dirty();
	bool dirty() const;
	void set_clean();

	void visible(bool v);

protected:
	/* Widget rectangle relative to parent pos(). */
	Rect parent_rect;
	const Style* style_ { nullptr };
	Widget* parent_ { nullptr };

	struct flags_t {
		bool dirty : 1;			// Widget content has changed.
		bool hidden : 1;		// Hide widget and children.
		bool focusable : 1;		// Widget can receive focus.
		bool highlighted : 1;	// Show in a highlighted style.
		bool visible : 1;		// Object was visible during last paint.
	};

	flags_t flags {
		.dirty = true,
		.hidden = false,
		.focusable = false,
		.highlighted = false,
		.visible = false,
	};
};

class View : public Widget {
public:
	View() {
	}

	View(Rect parent_rect) {
		set_parent_rect(parent_rect);
	}

	// TODO: ~View() should on_hide() all children?

	void set_parent_rect(const Rect new_parent_rect) override;

	void paint(Painter& painter) override;

	void add_child(Widget* const widget);
	void add_children(const std::vector<Widget*>& children);
	void remove_child(Widget* const widget);
	const std::vector<Widget*> children() const override;

	virtual Widget* initial_focus();

protected:
	std::vector<Widget*> children_;
	Rect dirty_screen_rect;

	void invalidate_child(Widget* const widget);
};

class Rectangle : public Widget {
public:
	constexpr Rectangle(
		const Rect parent_rect,
		const Color c
	) : Widget { parent_rect },
		color { c }
	{
	}

	void paint(Painter& painter) override;

	void set_color(const Color c);

private:
	Color color;
};

class Text : public Widget {
public:
	Text(
	) : text { "" } {
	}

	Text(
		Rect parent_rect,
		std::string text
	) : Widget { parent_rect },
		text { text }
	{
	}

	Text(
		Rect parent_rect
	) : Text { parent_rect, { } }
	{
	}

	void set(const std::string value);

	void paint(Painter& painter) override;

private:
	std::string text;
};

class Button : public Widget {
public:
	std::function<void(Button&)> on_select;

	Button(
		Rect parent_rect,
		std::string text
	) : Widget { parent_rect },
		text_ { text }
	{
		flags.focusable = true;
	}

	Button(
	) : Button { { }, { } }
	{
	}

	void set_text(const std::string value);
	std::string text() const;

	void paint(Painter& painter) override;

	bool on_key(const KeyEvent key) override;
	bool on_touch(const TouchEvent event) override;

private:
	std::string text_;
};

class OptionsField : public Widget {
public:
	using name_t = std::string;
	using value_t = int32_t;
	using option_t = std::pair<name_t, value_t>;
	using options_t = std::vector<option_t>;

	std::function<void(size_t, value_t)> on_change;

	OptionsField(
		Point parent_pos,
		size_t length,
		options_t options
	) : Widget { { parent_pos, { static_cast<ui::Dim>(8 * length), 16 } } },
		length_ { length },
		options { options }
	{
		flags.focusable = true;
	}

	size_t selected_index() const;
	void set_selected_index(const size_t new_index);

	void set_by_value(value_t v);

	void paint(Painter& painter) override;

	bool on_encoder(const EncoderEvent delta) override;
	bool on_touch(const TouchEvent event) override;

private:
	const size_t length_;
	options_t options;
	size_t selected_index_ { 0 };
};

class NumberField : public Widget {
public:
	std::function<void(int32_t)> on_change;

	using range_t = std::pair<int32_t, int32_t>;

	NumberField(
		Point parent_pos,
		size_t length,
		range_t range,
		int32_t step,
		char fill_char
	) : Widget { { parent_pos, { static_cast<ui::Dim>(8 * length), 16 } } },
		range { range },
		step { step },
		length_ { length },
		fill_char { fill_char }
	{
		flags.focusable = true;
	}

	NumberField(const NumberField&) = delete;
	NumberField(NumberField&&) = delete;

	int32_t value() const;
	void set_value(int32_t new_value);

	void paint(Painter& painter) override;

	bool on_encoder(const EncoderEvent delta) override;
	bool on_touch(const TouchEvent event) override;

private:
	const range_t range;
	const int32_t step;
	const size_t length_;
	const char fill_char;
	int32_t value_;

	int32_t clip_value(int32_t value);
};

} /* namespace ui */

#endif/*__UI_WIDGET_H__*/
