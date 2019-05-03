#pragma once

class State {
 public:
  State(int speed, float heading) : speed_(speed), heading_(heading) {}

  int getSpeed() { return speed_; }

  float getHeading() { return heading_; }

  void setSpeed(int speed) { speed_ = speed; }

  void setHeading(float heading) { heading_ = heading; }

  std::unique_ptr<State> clone() const {
    return std::make_unique<State>(this->speed_, this->heading_);
  }

 private:
  int speed_;
  float heading_;
};