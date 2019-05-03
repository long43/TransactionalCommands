// Copyright 2011- Facebook. All rights reserved.
#pragma once

#include "TransactionalCommand.h"

#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/futures/Future.h>
#include <memory>
#include <vector>

/**
 * An Executor takes a collection of the TransactionalCommand to execute in
 * serial, if one of the command return false or throw exception in middle, we
 * will rollback all executed commands.
 */
class TransactionExecutor {
 public:
  explicit TransactionExecutor(
      std::vector<std::shared_ptr<TransactionalCommand>>& commands);

  /**
   * By calling std::vector::clear() method, the destructors of
   * TransactionalCommand will be called one by one
   */
  ~TransactionExecutor();

  /**
   * Execute the vector of the commands. If in the middle one of the commands
   * failed, will rollback the all previous commands
   */
  bool executeCommands();

  folly::Future<bool> executeCommandsAsync(folly::CPUThreadPoolExecutor* exec);

  bool executeCommandsInParellel();

 private:
  std::vector<std::shared_ptr<TransactionalCommand>>& commands_;
};
