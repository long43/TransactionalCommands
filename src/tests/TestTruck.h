#include <functional>
#include <memory>

#include "State.h"

class TestTruck {
 public:
  explicit TestTruck(std::shared_ptr<State> state) : state_(state) {}

  int getSpeed() const { return state_->getSpeed(); }

  float getHeading() const { return state_->getHeading(); }

  const State& getState() { return *state_; }

  bool updateState(std::shared_ptr<State> state) {
    state_ = state;
    return true;
  }

  bool updateStateThrowsException(std::shared_ptr<State> state) {
    state_ = state;
    throw std::runtime_error("runtime error");
  }

  bool updateStateReturnsFalse(std::shared_ptr<State> state) {
    state_ = state;
    return false;
  }

  auto getUpdateStateFunc(TestTruck* car, std::shared_ptr<State> state)
      -> decltype(std::bind(&TestTruck::updateState, car, state)) {
    return std::bind(&TestTruck::updateState, car, state);
  }

  auto getUpdateStateThrowsExceptionFunc(TestTruck* car,
                                         std::shared_ptr<State> state)
      -> decltype(std::bind(&TestTruck::updateStateThrowsException, car,
                            state)) {
    return std::bind(&TestTruck::updateStateThrowsException, car, state);
  }

  auto getUpdateStateReturnsFalseFunc(TestTruck* car,
                                      std::shared_ptr<State> state)
      -> decltype(std::bind(&TestTruck::updateStateReturnsFalse, car, state)) {
    return std::bind(&TestTruck::updateStateReturnsFalse, car, state);
  }

 private:
  std::shared_ptr<State> state_;
};