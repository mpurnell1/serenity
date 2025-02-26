/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Layout/BlockContainer.h>

namespace Web::Layout {

class TableRowGroupBox final : public BlockContainer {
    JS_CELL(TableRowGroupBox, BlockContainer);

public:
    TableRowGroupBox(DOM::Document&, DOM::Element*, NonnullRefPtr<CSS::StyleProperties>);
    virtual ~TableRowGroupBox() override;
};

}
