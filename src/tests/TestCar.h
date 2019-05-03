
#include <folly/Function.h>
#include <memory>

#include "State.h"

class TestCar {
 public:
  explicit TestCar(std::unique_ptr<State> state);

  int getSpeed() const;

  float getHeading() const;

  const State& getState();

  bool updateState(std::unique_ptr<State> state);

  bool updateStateThrowsException(std::unique_ptr<State> state);

  bool updateStateReturnsFalse(std::unique_ptr<State> state);

  folly::Function<bool()> getUpdateStateFunc(TestCar* car,
                                             std::unique_ptr<State> state);

  folly::Function<bool()> getUpdateStateThrowsExceptionFunc(
      TestCar* car, std::unique_ptr<State> state);

  folly::Function<bool()> getUpdateStateReturnsFalseFunc(
      TestCar* car, std::unique_ptr<State> state);

 private:
  std::unique_ptr<State> state_;
};