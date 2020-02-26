/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/automatic/AlignAngle.h"
#include "subsystems/Drivetrain.h"

double constrainAngle(double input) {
  return std::fmod(std::fmod(input, 360.0) + 360.0, 360.0);
}

double Aligner::getAngle() const {
  double angle = drivetrain->GetPose().Rotation().Degrees().to<double>();
  return constrainAngle(angle);
}

void Aligner::setOutput(double output) {
  drivetrain->SetSpeeds(output, -output);
}

void Aligner::update(double target) {
  double output = Calculate(getAngle(), constrainAngle(target));

  if (output > 0.2) {
    output = 0.2;
  } else if (output < -0.2) {
    output = -0.2;
  }

  setOutput(output);
}

Aligner::Aligner(Drivetrain *drivetrain) :
  frc2::PIDController(0.015, 0.0, 0.0),
  drivetrain(drivetrain)
{
  EnableContinuousInput(0.0, 360.0);
  SetTolerance(1.0);
  Reset();
}

AlignAngle::AlignAngle(units::degree_t target, Drivetrain *drivetrain) :
  drivetrain(drivetrain),
  target(target),
  aligner(drivetrain)
{
  AddRequirements(drivetrain);
}

AlignAngle::AlignAngle(units::degree_t *target, Drivetrain *drivetrain) :
  drivetrain(drivetrain),
  target2(target),
  aligner(drivetrain)
{
  AddRequirements(drivetrain);
}

// Called when the command is initially scheduled.
void AlignAngle::Initialize() {
  aligner.Reset();
  successes = 0;
}

units::degree_t AlignAngle::GetTarget() const {
  double temp;
  if (target2 != nullptr) {
    temp = target2->to<double>();
  } else {
    temp = target.to<double>();
  }

  return units::degree_t(constrainAngle(temp));
}

double AlignAngle::GetAngleError() const {
  units::degree_t drivetrainAngle = drivetrain->GetPose().Rotation().Degrees();

  double diff = (GetTarget() - drivetrainAngle).to<double>();

  // Correct differences so they're in the range -180, 180
  diff -= floor((diff + 180.0) / 360.0) * 360.0;
  
  return diff;
}

// Called repeatedly when this Command is scheduled to run
void AlignAngle::Execute() {
  double diff = GetAngleError();

  const double TURN_SPEED = 0.1;
  const double MAX_ERROR = 5.0;
  if (diff > MAX_ERROR) {
    drivetrain->SetSpeeds(TURN_SPEED, -TURN_SPEED);
  } else if (diff < -MAX_ERROR) {
    drivetrain->SetSpeeds(-TURN_SPEED, TURN_SPEED);
  } else {
    drivetrain->SetSpeeds(0.0, 0.0);
    successes += 1;
  }
}

// Called once the command ends or is interrupted.
void AlignAngle::End(bool interrupted) {
  drivetrain->SetSpeeds(0.0, 0.0);
}

// Returns true when the command should end.
bool AlignAngle::IsFinished() {
  return successes >= 5;
}