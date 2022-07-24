#ifndef VEVAH_VIEW_SELECTED_LISTENER_H_
#define VEVAH_VIEW_SELECTED_LISTENER_H_


namespace ukive {
    class View;
}

namespace vevah {

    class OnViewSelectedListener {
    public:
        virtual ~OnViewSelectedListener() = default;

        virtual void onViewSelected(ukive::View* v) {}
    };

}

#endif  // VEVAH_VIEW_SELECTED_LISTENER_H_