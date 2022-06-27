#include "property_page.h"

#include "vevah/resources/necro_resources_id.h"
#include "ukive/resources/layout_parser.h"


namespace vevah {

    PropertyPage::PropertyPage() {}

    ukive::View* PropertyPage::onCreate(ukive::LayoutView* parent) {
        return ukive::LayoutParser::from(
            parent->getContext(),
            parent, Res::Layout::property_page_layout_xml);
    }

    void PropertyPage::onCreated(ukive::View* v) {}

    void PropertyPage::onShow(bool show) {}

    void PropertyPage::onDestroy() {}

}