/*
 * Copyright (C) 2020 Damir Porobic <damir.porobic@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ListMenu.h"

namespace kImageAnnotator {

ListMenu::ListMenu(QWidget *parent) :
	QMenu(parent),
	mContainer(new QWidget(this)),
	mLayout(new QVBoxLayout(mContainer)),
	mWidgetAction(new QWidgetAction(this)),
	mListItemGroup(new ListItemGroup(this))
{
	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->setSpacing(0);

	// With Qt 6.8 on macOS the layout of a widget in a menu starts 34 points below the top
	// of the widget and only gets the rest of its height, so the items were squeezed. Laying
	// out on the whole rect avoids that, it makes no difference with Qt 6.12.
	mContainer->setAttribute(Qt::WA_LayoutOnEntireRect, true);

	// The items are added to the menu through a widget action. A layout that is set
	// on the menu itself does not get the full height of the menu with Qt 6 on macOS,
	// so the items were squeezed and cut off.
	mWidgetAction->setDefaultWidget(mContainer);
	addAction(mWidgetAction);

	connect(mListItemGroup, &ListItemGroup::checkedItemChanged, this, &ListMenu::selectionChanged);
}

ListMenu::~ListMenu()
{
	qDeleteAll(mListItemGroup->items());
}

void ListMenu::addItem(const QIcon &icon, const QString &text, const QVariant &data)
{
	auto item = new ListMenuItem(icon, text, data, mContainer);
	mLayout->addWidget(item);
	mListItemGroup->addItem(item);
}

QVariant ListMenu::currentData() const
{
	auto item = mListItemGroup->checkedItem();
	return item != nullptr ? item->data() : QVariant();
}

void ListMenu::setCurrentData(const QVariant &data)
{
	for(auto item : mListItemGroup->items()) {
		if(item->data() == data){
			item->setIsChecked(true);
			return;
		}
	}
}

QIcon ListMenu::currentIcon() const
{
	auto item = mListItemGroup->checkedItem();
	return item != nullptr ? item->icon() : QIcon();
}

QString ListMenu::currentText() const
{
	auto item = mListItemGroup->checkedItem();
	return item != nullptr ? item->text() : QString();
}

void ListMenu::setDataVisible(const QVariant &data, bool isVisible)
{
	for(auto item : mListItemGroup->items()) {
		if(item->data() == data){
			item->setVisible(isVisible);
			updateSize();
			return;
		}
	}
}

/*
 * The menu only recalculates the size of its actions when they change, so the
 * widget action is added again after the visible items changed.
 */
void ListMenu::updateSize()
{
	mContainer->adjustSize();
	removeAction(mWidgetAction);
	addAction(mWidgetAction);
}

} // namespace kImageAnnotator
