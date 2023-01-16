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