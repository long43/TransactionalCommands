#include "TransactionalCommand.h"

#include <glog/logging.h>

using std::function;
using std::string;

TransactionalCommand::TransactionalCommand(const string name,
                                           folly::Function<bool()>&& execute,
                                           folly::Function<bool()>&& rollback)
    : name_(name),
      committed_(false),
      executed_(false),
      execute_(std::move(execute)),
      rollback_(std::move(rollback)) {}

TransactionalCommand::~TransactionalCommand() {
  // only rollback the executed tasks
  if (executed_ && !committed_) {
    LOG(INFO) << "rollback the command " << name_;
    // DO NOT throw exception as we'd rather have inconsistent states than crash
    // the entire application
    try {
      rollback_();
    } catch (std::exception& err) {
      LOG(WARNING) << "rollback command " << name_
                   << " failed due to the error " << err.what();
    }
  }
}

string TransactionalCommand::getName() const { return name_; }

void TransactionalCommand::commit() noexcept { committed_ = true; }

bool TransactionalCommand::execute() {
  LOG(INFO) << "execute the command " << name_;
  executed_ = true;
  return execute_();
}