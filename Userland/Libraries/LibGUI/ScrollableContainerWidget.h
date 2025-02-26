/*
 * Copyright (c) 2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/AbstractScrollableWidget.h>

namespace GUI {

class ScrollableContainerWidget : public GUI::AbstractScrollableWidget {
    C_OBJECT(ScrollableContainerWidget);

public:
    virtual ~ScrollableContainerWidget() = default;

    void set_widget(GUI::Widget*);
    GUI::Widget* widget() { return m_widget; }
    GUI::Widget const* widget() const { return m_widget; }

protected:
    virtual void did_scroll() override;
    virtual void resize_event(GUI::ResizeEvent&) override;
    virtual void layout_relevant_change_occurred() override;

private:
    void update_widget_size();
    void update_widget_position();
    void update_widget_min_size();
    virtual bool load_from_gml_ast(NonnullRefPtr<GUI::GML::Node> ast, RefPtr<Core::Object> (*unregistered_child_handler)(DeprecatedString const&)) override;

    ScrollableContainerWidget();

    RefPtr<GUI::Widget> m_widget;
};

}
