#include <folly/ScopeGuard.h>
#include <gtest/gtest.h>

#include "../transactional/TransactionExecutor.h"
#include "../transactional/TransactionalCommand.h"
#include "TestCar.h"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

TEST(TransactionalCommandTest, UpdateAndCommitAll) {
  unique_ptr<State> oldState1 = std::make_unique<State>(0, 0);
  unique_ptr<State> oldState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> oldState3 = std::make_unique<State>(20, 18.9);

  unique_ptr<State> newState1 = std::make_unique<State>(40, 27);
  unique_ptr<State> newState2 = std::make_unique<State>(10, 82.8);
  unique_ptr<State> newState3 = std::make_unique<State>(20, 18.9);

  TestCar car1(std::move(oldState1->clone()));
  TestCar car2(std::move(oldState2->clone()));
  TestCar car3(std::move(oldState3->clone()));

  SCOPE_EXIT {
    ASSERT_EQ(car1.getSpeed(), newState1->getSpeed());
    ASSERT_FLOAT_EQ(car2.getHeading(), newState2->getHeading());
    ASSERT_EQ(car3.getSpeed(), newState3->getSpeed());
  };

  auto cmd1 = std::make_shared<TransactionalCommand>(
      "UpdateCar1State", car1.getUpdateStateFunc(&car1, newState1->clone()),
      car1.getUpdateStateFunc(&car1, oldState1->clone()));

  auto cmd2 = std::make_shared<TransactionalCommand>(
      "UpdateCar2State", car2.getUpdateStateFunc(&car2, newState2->clone()),
      car2.getUpdateStateFunc(&car2, oldState2->clone()));

  auto cmd3 = std::make_shared<TransactionalCommand>(
      "UpdateCar3State", car3.getUpdateStateFunc(&car3, newState3->clone()),
      car3.getUpdateStateFunc(&car3, oldState3->clone()));

  vector<shared_ptr<TransactionalCommand>> commands{cmd1, cmd2, cmd3};
  TransactionExecutor exe{commands};
  exe.executeCommands();
}

void testRollbackWithException(TestCar& car1, TestCar& car2, TestCar& car3) {}

TEST(TransactionalCommandTest, UpdateAndExceptionThrownInTheEnd) {
  unique_ptr<State> oldState1 = std::make_unique<State>(0, 0);
  unique_ptr<State> oldState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> oldState3 = std::make_unique<State>(20, 18.9);

  TestCar car1(std::move(oldState1->clone()));
  TestCar car2(std::move(oldState2->clone()));
  TestCar car3(std::move(oldState3->clone()));

  unique_ptr<State> newState1 = std::make_unique<State>(40, 27);
  unique_ptr<State> newState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> newState3 = std::make_unique<State>(20, 18.9);

  SCOPE_EXIT {
    ASSERT_EQ(car1.getSpeed(), oldState1->getSpeed());
    ASSERT_FLOAT_EQ(car2.getHeading(), oldState2->getHeading());
    ASSERT_EQ(car3.getSpeed(), oldState3->getSpeed());
  };

  auto cmd1 = std::make_shared<TransactionalCommand>(
      "UpdateCar1State", car1.getUpdateStateFunc(&car1, newState1->clone()),
      car1.getUpdateStateFunc(&car1, car1.getState().clone()));

  auto cmd2 = std::make_shared<TransactionalCommand>(
      "UpdateCar2State", car2.getUpdateStateFunc(&car2, newState1->clone()),
      car2.getUpdateStateFunc(&car2, car2.getState().clone()));

  auto cmd3 = std::make_shared<TransactionalCommand>(
      "UpdateCar3StateThrowException",
      car3.getUpdateStateThrowsExceptionFunc(&car3, newState3->clone()),
      car3.getUpdateStateFunc(&car3, car3.getState().clone()));

  vector<shared_ptr<TransactionalCommand>> commands{cmd1, cmd2, cmd3};
  TransactionExecutor exe{commands};
  exe.executeCommands();
}

void testRollbackWithReturnFalse(TestCar& car1, TestCar& car2, TestCar& car3) {
  unique_ptr<State> newState1 = std::make_unique<State>(40, 27);
  unique_ptr<State> newState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> newState3 = std::make_unique<State>(20, 18.9);

  auto cmd1 = std::make_shared<TransactionalCommand>(
      "UpdateCar1State", car1.getUpdateStateFunc(&car1, newState1->clone()),
      car1.getUpdateStateFunc(&car1, car1.getState().clone()));

  auto cmd2 = std::make_shared<TransactionalCommand>(
      "UpdateCar2State", car2.getUpdateStateFunc(&car2, newState1->clone()),
      car2.getUpdateStateFunc(&car2, car2.getState().clone()));

  auto cmd3 = std::make_shared<TransactionalCommand>(
      "UpdateCar3StateThrowException",
      car3.getUpdateStateReturnsFalseFunc(&car3, newState3->clone()),
      car3.getUpdateStateFunc(&car3, car3.getState().clone()));

  vector<shared_ptr<TransactionalCommand>> commands{cmd1, cmd2, cmd3};
  TransactionExecutor exe{commands};
  exe.executeCommands();
}

TEST(TransactionalCommandTest, UpdateAndReturnFalseInTheEnd) {
  unique_ptr<State> oldState1 = std::make_unique<State>(0, 0);
  unique_ptr<State> oldState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> oldState3 = std::make_unique<State>(20, 18.9);

  TestCar car1(std::move(oldState1->clone()));
  TestCar car2(std::move(oldState2->clone()));
  TestCar car3(std::move(oldState3->clone()));

  testRollbackWithReturnFalse(car1, car2, car3);

  ASSERT_EQ(car1.getSpeed(), oldState1->getSpeed());
  ASSERT_FLOAT_EQ(car2.getHeading(), oldState2->getHeading());
  ASSERT_EQ(car3.getSpeed(), oldState3->getSpeed());
}

void testRollbackWithExceptionRollbackFailed(TestCar& car1, TestCar& car2,
                                             TestCar& car3) {}

TEST(TransactionalCommandTest, UpdateAndThrowExceptionRollbackFailed) {
  unique_ptr<State> oldState1 = std::make_unique<State>(0, 0);
  unique_ptr<State> oldState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> oldState3 = std::make_unique<State>(20, 18.9);

  TestCar car1(std::move(oldState1->clone()));
  TestCar car2(std::move(oldState2->clone()));
  TestCar car3(std::move(oldState3->clone()));

  unique_ptr<State> newState1 = std::make_unique<State>(40, 27);
  unique_ptr<State> newState2 = std::make_unique<State>(10, 9.2);
  unique_ptr<State> newState3 = std::make_unique<State>(20, 18.9);

  SCOPE_EXIT {
    ASSERT_EQ(car1.getSpeed(), oldState1->getSpeed());
    // testCar2 rollback failed, the speed will be still 65
    ASSERT_EQ(car2.getSpeed(), newState2->getSpeed());
    ASSERT_EQ(car3.getSpeed(), oldState3->getSpeed());
  };

  auto cmd1 = std::make_shared<TransactionalCommand>(
      "UpdateCar1State", car1.getUpdateStateFunc(&car1, newState1->clone()),
      car1.getUpdateStateFunc(&car1, car1.getState().clone()));

  auto cmd2 = std::make_shared<TransactionalCommand>(
      "UpdateCar2State", car2.getUpdateStateFunc(&car2, newState1->clone()),
      car2.getUpdateStateThrowsExceptionFunc(&car2, car2.getState().clone()));

  auto cmd3 = std::make_shared<TransactionalCommand>(
      "UpdateCar3StateThrowException",
      car3.getUpdateStateThrowsExceptionFunc(&car3, newState3->clone()),
      car3.getUpdateStateFunc(&car3, car3.getState().clone()));

  vector<shared_ptr<TransactionalCommand>> commands{cmd1, cmd2, cmd3};
  TransactionExecutor exe{commands};
  exe.executeCommands();
}