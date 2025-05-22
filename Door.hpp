#ifndef DOOR_HPP
#define DOOR_HPP

#include "Model.hpp"

// emum for door state
enum class DoorState {
    Closed,
    Opening,
    Opened,
    Closing
};

class Door : public Model {
public:
    DoorState state = DoorState::Closed;
    float open_progress = 0.0f;
    float open_speed = 0.8f; // units/sec
    float open_distance = 1.01f; // jak hluboko dveře zajíždí
    float original_y = 0.0f;

    Door() = default;
    Door(const Model& base) : Model(base) {
        original_y = origin.y;
    }

    void startOpening() { 
        if (state == DoorState::Closed) {
            state = DoorState::Opening;
        }
    }

    void startClosing() { 
        if (state == DoorState::Opened) {
            state = DoorState::Closing;
        }
    }

    void interact() {
        if (state == DoorState::Closed) {
            startOpening();
        } else if (state == DoorState::Opened) {
            startClosing();
        }
    }

    void update(float delta_t) {
        switch (state) {
            case DoorState::Opening:
                open_progress += open_speed * delta_t;
                if (open_progress >= open_distance) {
                    open_progress = open_distance;
                    state = DoorState::Opened;
                    std::cout << "Door opened" << std::endl;
                }
                origin.y = original_y - open_progress;
                break;
            case DoorState::Closing:
                open_progress -= open_speed * delta_t;
                if (open_progress <= 0.0f) {
                    open_progress = 0.0f;
                    state = DoorState::Closed;
                    std::cout << "Door closed" << std::endl;
                }
                origin.y = original_y - open_progress;
                break;
            case DoorState::Closed:
                origin.y = original_y;
                break;
            case DoorState::Opened:
                origin.y = original_y - open_distance;
                break;
            default:
                break;
        }
    }
};

#endif // DOOR_HPP