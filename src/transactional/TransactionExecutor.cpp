#include "TransactionExecutor.h"

#include <glog/logging.h>

using folly::CPUThreadPoolExecutor;
using folly::Future;
using std::shared_ptr;
using std::vector;

TransactionExecutor::TransactionExecutor(
    vector<shared_ptr<TransactionalCommand>>& commands)
    : commands_(commands){};

TransactionExecutor::~TransactionExecutor() { commands_.clear(); }

Future<bool> TransactionExecutor::executeCommandsAsync(
    CPUThreadPoolExecutor* exec) {
  return folly::makeFuture().via(exec).thenValue(
      [=](auto&&) { return executeCommands(); });
}

bool TransactionExecutor::executeCommandsInParellel() {
  folly::CPUThreadPoolExecutor executor(4);
  vector<Future<bool>> fs;
  for (auto& command : commands_) {
    Future<bool> future =
        folly::via(&executor, [=]() { return command->execute(); });

    fs.emplace_back(std::move(future));
  }

  bool succeed = collectAll(fs)
                     .thenValue([](const std::vector<folly::Try<bool>>& tries) {
                       for (const auto& cmdTry : tries) {
                         if (!cmdTry.hasValue()) {
                           LOG(ERROR) << "Command Error, rollback....";
                           return false;
                         } else {
                           if (cmdTry.value() == false) {
                             return false;
                           }
                         }
                       }
                       return true;
                     })
                     .get();

  if (succeed) {
    // only commit after the commands were succeesfully executed
    for (auto& command : commands_) {
      command->commit();
    }
  }

  return succeed;
}

bool TransactionExecutor::executeCommands() {
  try {
    bool succeed = true;
    for (auto& command : commands_) {
      if (!command->execute()) {
        succeed = false;
        break;
      }
    }

    if (succeed) {
      // only commit after the commands were succeesfully executed
      for (auto& command : commands_) {
        command->commit();
      }
    }

    return succeed;
  } catch (...) {
    LOG(ERROR) << "Command Error, rollback....";
    return false;
  }
}
