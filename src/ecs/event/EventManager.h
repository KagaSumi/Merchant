//
// Created by Curry on 2026-02-04.
//

#ifndef PROJECT_EVENTMANAGER_H
#define PROJECT_EVENTMANAGER_H
#include <functional>
#include "BaseEvent.h"


//observer pattern
class EventManager {
public:
    using Handler = std::function<void(const BaseEvent&)>;
    // template<typename EventType>
    // void emit(const EventType& event) {
    //     //retrieve the list of subscribers to certain events
    //     auto& listeners = getListeners<EventType>();
    //     //loop all subscribers to certain events
    //     for (auto& listener : listeners) {
    //         listener(event); // invoke function
    //     }
    // }

    void emit(const BaseEvent& event) const{
        for (const auto& listener : listeners) {
            listener(event);
        }
    }

    /*template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback) {
        //pass in callback/callable wrapper/subscription to the list of subscriptions
        getListeners<EventType>().push_back(callback);
    }*/

    void subscribe(const Handler& callback) {
        listeners.emplace_back(callback);
    }

private:
    //Each event type essentially has its own std:vector of listeners without you having to manage it explicitly.
    //This is done using static local.
    //std::function<void(const EventType&)> is a callable wrapper: can hold anything that can be called like a function
    //(Lambda, free function, functor, etc.)
    /* template<typename EventType>
    // std::vector<std::function<void(const EventType&)>>& getListeners() {
    //     static std::vector<std::function<void(const EventType&)>> listeners;
    //     return listeners;
    } */

    std::vector<Handler> listeners;
};
#endif //PROJECT_EVENTMANAGER_H