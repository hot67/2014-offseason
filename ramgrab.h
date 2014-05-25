#ifndef RAMGRAB_H
#define RAMGRAB_H

#include "WPILib.h"
#include "RobotUtils/RobotUtils.h"

#define LATCH_CHANNEL 10
#define RAM_MOTOR_CHANNEL 5
#define ROLLER_CHANNEL 7
#define B_ARM_CHANNEL 2
#define CATCH_ARM_CHANNEL 3
#define RAM_ENCODE_CHANNEL_ONE 7
#define RAM_ENCODE_CHANNEL_TWO 8
#define ROLL_TIMEOUT 0.5

class RamGrab: public HotSubsystem
{
public:
    enum roll_t {
        kIn,
        kOut,
        kOff
    };

    friend class HotSubsystemHandler;

    RamGrab();

    void Shoot ();
    void MedShoot ();
    void Reload ();

    void Roll(roll_t direction);

    void CatchArms (bool open) { m_catchArms->Set(open); }
    void bArm (bool open) { m_bArm->Set(open); }

    void SetTimedOff(float timeout=ROLL_TIMEOUT);

    bool Reloading() { return m_ramCase > 3; }

protected:
    void Update();
private:
    void RamFire();
    void MedRamFire();
    void RollTime();

    int m_ramCase, m_medRamCase;
    float m_rollTimeout;

    Timer* m_rollTime;
    Timer* m_ramTime;

    Servo* m_ramLatch;
    Talon* m_ramMotor;
    Talon* m_roller;
    Solenoid* m_catchArms;
    Solenoid* m_bArm;

    Encoder* m_ramEncode;
};

#endif // RAMGRAB_H
