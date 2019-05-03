// Copyright 2011- Facebook. All rights reserved.

#pragma once

#include <folly/Function.h>
#include <string>
#include <vector>

/**
 * Wraps a command/function into a transaction. Inside a transaction, we use two
 * state variable "executed" and "committed" to track the states of the command.
 * When the destructor hitted, if the command is executed but not committed yet,
 * it will be rolled back automatically. Note that we should always assume the
 * rollback method is exception safe.
 */
class TransactionalCommand {
 public:
  explicit TransactionalCommand(const std::string name,
                                folly::Function<bool()>&& execute,
                                folly::Function<bool()>&& rollback);

  /**
   * When destructor is called, we should check if the command is executed and
   * committed. If it's executed but not committed, we rollback the command.
   */
  ~TransactionalCommand();

  /**
   * Get the name of the command
   */
  std::string getName() const;

  /**
   * Execute the command, mark the executed_ to true. Return true if the command
   * succeed, otherwise return false
   */
  bool execute();

  /**
   * Commit the command by mark the committed_ to true. In destructor, it will
   * be examined and if it's not committed yet, rollback the command
   */
  void commit() noexcept;

 protected:
  std::string name_;
  bool committed_;
  bool executed_;
  folly::Function<bool()> execute_;
  folly::Function<bool()> rollback_;
};
