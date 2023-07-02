// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


#include "string_template.h"

PlainStringTemplateItem::PlainStringTemplateItem(std::string value) {
    this->value = value;
};

InsertionTemplateItem::InsertionTemplateItem(u32 index) {
    this->index = index;
};

std::string PlainStringTemplateItem::render(std::vector<std::string> inersions) {
    return value;
};

std::string InsertionTemplateItem::render(std::vector<std::string> inersions) {
    return inersions[index];
};

StringTemplate::StringTemplate() {
    items = std::vector<StringTemplateItem*>();
};

StringTemplate::~StringTemplate() {
    for (StringTemplateItem* item : items) {
        delete item;
    }
};

void StringTemplate::push(StringTemplateItem* item) {
    items.push_back(item);
}

std::string StringTemplate::render(std::vector<std::string> inersions) {
    std::string result = "";
    for (StringTemplateItem* item : items) {
        result += item->render(inersions);
    }
    return result;
};
