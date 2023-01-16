#pragma once

#include <string>
#include <vector>

#include "types.h"

class StringTemplateItem {
    public:
        virtual std::string render(std::vector<std::string> inersions) { return ""; };
};

class PlainStringTemplateItem : public StringTemplateItem {
    private:
        std::string value;
    public:
        PlainStringTemplateItem(std::string value);
        std::string render(std::vector<std::string> inersions) override;
};

class InsertionTemplateItem : public StringTemplateItem {
    private:
        u32 index;
    public:
        InsertionTemplateItem(u32 index);
        std::string render(std::vector<std::string> inersions) override;
};

class StringTemplate {
    private:
        std::vector<StringTemplateItem*> items;
    public:
        StringTemplate();
        ~StringTemplate();
        
        std::string render(std::vector<std::string> inersions);
        void push(StringTemplateItem* item);
};