/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>
#include "Input.h"

class Drivetrain;

namespace frc {
  class XboxController;
}

/**
 * An example command.
 *
 * <p>Note that this extends CommandHelper, rather extending CommandBase
 * directly; this is crucially important, or else the decorator functions in
 * Command will *not* work!
 */
class ManualDrive
    : public frc2::CommandHelper<frc2::CommandBase, ManualDrive> {
 public:
  ManualDrive(Drivetrain *drivetrain, Input *xboxController);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

  void setConstantSpeed(double speed);
 
 private:
  Drivetrain *drivetrain;
  Input *input;

  bool useConstantSpeed;
  double constantSpeed;
};
