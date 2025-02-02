/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright © 2019 by The qTox Project Contributors
 * Copyright © 2024-2025 The TokTok team.
 */

#include "genericchatitemlayout.h"
#include "genericchatitemwidget.h"
#include <QBoxLayout>
#include <QCollator>
#include <cassert>

// As this layout sorts widget, extra care must be taken when inserting widgets.
// Prefer using the build in add and remove functions for modifying widgets.
// Inserting widgets other ways would cause this layout to be unable to sort.
// As such, they are protected using asserts.

GenericChatItemLayout::GenericChatItemLayout()
    : layout(new QVBoxLayout())
{
}

GenericChatItemLayout::~GenericChatItemLayout()
{
    delete layout;
}

void GenericChatItemLayout::addSortedWidget(GenericChatItemWidget* widget, int stretch,
                                            Qt::Alignment alignment)
{
    int closest = indexOfClosestSortedWidget(widget);
    layout->insertWidget(closest, widget, stretch, alignment);
}

int GenericChatItemLayout::indexOfSortedWidget(GenericChatItemWidget* widget) const
{
    if (layout->isEmpty())
        return -1;

    int index = indexOfClosestSortedWidget(widget);

    if (index >= layout->count())
        return -1;

    GenericChatItemWidget* atMid =
        qobject_cast<GenericChatItemWidget*>(layout->itemAt(index)->widget());
    assert(atMid != nullptr);

    if (atMid == widget)
        return index;

    return -1;
}

bool GenericChatItemLayout::existsSortedWidget(GenericChatItemWidget* widget) const
{
    return indexOfSortedWidget(widget) != -1;
}

void GenericChatItemLayout::removeSortedWidget(GenericChatItemWidget* widget)
{
    if (layout->isEmpty())
        return;

    int index = indexOfClosestSortedWidget(widget);

    if (layout->itemAt(index) == nullptr)
        return;

    GenericChatItemWidget* atMid =
        qobject_cast<GenericChatItemWidget*>(layout->itemAt(index)->widget());
    assert(atMid != nullptr);

    if (atMid == widget)
        layout->removeWidget(widget);
}

void GenericChatItemLayout::search(const QString& searchString, bool hideAll)
{
    for (int index = 0; index < layout->count(); ++index) {
        GenericChatItemWidget* widgetAt =
            qobject_cast<GenericChatItemWidget*>(layout->itemAt(index)->widget());
        assert(widgetAt != nullptr);

        widgetAt->searchName(searchString, hideAll);
    }
}

QLayout* GenericChatItemLayout::getLayout() const
{
    return layout;
}

int GenericChatItemLayout::indexOfClosestSortedWidget(GenericChatItemWidget* widget) const
{
    // Binary search: Deferred test of equality.
    int min = 0, max = layout->count();
    while (min < max) {
        int mid = (max - min) / 2 + min;
        GenericChatItemWidget* atMid =
            qobject_cast<GenericChatItemWidget*>(layout->itemAt(mid)->widget());
        assert(atMid != nullptr);

        bool lessThan = false;

        QCollator collator;
        collator.setNumericMode(true);

        int compareValue = collator.compare(atMid->getName(), widget->getName());

        if (compareValue < 0)
            lessThan = true;
        else if (compareValue == 0)
            lessThan = atMid < widget; // Consistent ordering.

        if (lessThan)
            min = mid + 1;
        else
            max = mid;
    }
    return min;
}
