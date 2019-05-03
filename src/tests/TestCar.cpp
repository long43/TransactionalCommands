#include "TestCar.h"

TestCar::TestCar(std::unique_ptr<State> state) : state_(std::move(state)) {}

int TestCar::getSpeed() const { return state_->getSpeed(); }

float TestCar::getHeading() const { return state_->getHeading(); }

const State& TestCar::getState() { return *state_; }

bool TestCar::updateState(std::unique_ptr<State> state) {
  state_ = std::move(state);
  return true;
}

bool TestCar::updateStateThrowsException(std::unique_ptr<State> state) {
  state_ = std::move(state);
  throw std::runtime_error("runtime error");
}

bool TestCar::updateStateReturnsFalse(std::unique_ptr<State> state) {
  state_ = std::move(state);
  return false;
}

folly::Function<bool()> TestCar::getUpdateStateFunc(
    TestCar* car, std::unique_ptr<State> state) {
  return [car, st = std::move(state)]() mutable -> bool {
    return car->updateState(std::move(st));
  };
}

folly::Function<bool()> TestCar::getUpdateStateThrowsExceptionFunc(
    TestCar* car, std::unique_ptr<State> state) {
  return [car, st = std::move(state)]() mutable -> bool {
    return car->updateStateThrowsException(std::move(st));
  };
}

folly::Function<bool()> TestCar::getUpdateStateReturnsFalseFunc(
    TestCar* car, std::unique_ptr<State> state) {
  return [car, st = std::move(state)]() mutable -> bool {
    return car->updateStateReturnsFalse(std::move(st));
  };
}