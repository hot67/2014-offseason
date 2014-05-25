#include "ramgrab.h"

RamGrab::RamGrab() : HotSubsystem("RamGrab")
{
    m_ramCase = -1;
    m_medRamCase = -1;

    m_rollTime = new Timer;
    m_ramTime = new Timer;

    m_roller = new Talon (ROLLER_CHANNEL);
    m_ramMotor = new Talon (RAM_MOTOR_CHANNEL);

    m_bArm = new Solenoid (B_ARM_CHANNEL);
    m_catchArms = new Solenoid (CATCH_ARM_CHANNEL);

    m_ramLatch = new Servo (LATCH_CHANNEL);

    m_ramEncode = new Encoder(RAM_ENCODE_CHANNEL_ONE, RAM_ENCODE_CHANNEL_TWO, true);
    m_ramEncode->SetDistancePerPulse(1);
    m_ramEncode->SetMaxPeriod(1.0);
    m_ramEncode->Start();
}

void RamGrab::Shoot()
{
    m_ramCase = 0;
}

void RamGrab::MedShoot()
{
    m_medRamCase = 0;
}

void RamGrab::RamFire()
{
    switch (m_ramCase) {
    case 0:
        m_ramTime->Stop();
        m_ramTime->Start();
        m_ramTime->Reset();

        m_roller->Set(-1.0);

        m_bArm->Set(true);

        m_ramCase++;
        break;
    case 1:
        m_ramLatch->SetAngle(180);
        if (m_ramTime->Get() > 0.15) {
            m_roller->Set(0.0);
        }
        if (m_ramTime->HasPeriodPassed(1.0))
            m_ramCase++;
        break;
    case 2:
        m_ramLatch->SetAngle(0);
        m_ramCase++;
        m_ramTime->Stop();
        m_ramTime->Start();
        m_ramTime->Reset();
        m_ramMotor->Set(0.8);
        break;
    case 3:
        if (m_ramTime->HasPeriodPassed(0.1)) {
            if (abs(m_ramEncode->GetRate()) < 20) {
                m_ramMotor->Set(0.0);
                m_ramTime->Stop();
                m_ramTime->Start();
                m_ramTime->Reset();
                m_ramCase++;
            }
        }
        break;
    case 4:
        if (m_ramTime->HasPeriodPassed(0.1)) {
            if (abs(m_ramEncode->GetDistance()) < 200) {
                m_ramMotor->Set(-0.15);
                m_bArm->Set(false);
                m_ramCase++;
            } else
                m_ramMotor->Set(-0.4);
        }
        break;
    case 5:
        if (abs(m_ramEncode->GetRate()) < 5) {
            m_ramEncode->Reset();
            m_ramMotor->Set(0.0);
            m_ramTime->Stop();
            m_ramTime->Start();
            m_ramTime->Reset();
            m_ramCase = -1;
        }
        break;
    }
}

void RamGrab::MedRamFire()
{
    switch (m_medRamCase) {
    case 0:
        if (abs(m_ramEncode->GetDistance()) > (RAM_MID_POSITION - 100))
            m_ramMotor->Set(1);
        else
            m_medRamCase++;
        break;
    case 1:
        if (abs(m_ramEncode->GetDistance()) > (RAM_MID_POSITION - 20)) {
            m_ramMotor->Set(0.2);
            m_medRamCase++;
        } else
            m_ramMotor->Set(1.0);
        break;
    case 2:
        if (m_ramEncode->GetDistance() > (RAM_MID_POSITION - 50)) {
            m_ramMotor->Set(0.0);
            m_ramTime->Stop();
            m_ramTime->Start();
            m_ramTime->Reset();
            m_medRamCase++;
        }
        break;
    case 3:
        if (m_ramLatch->GetAngle() != 120)
            m_ramLatch->SetAngle(120);
        if (m_ramTime->HasPeriodPassed(0.3)) {
            m_ramTime->Stop();
            m_ramTime->Reset();
            m_medRamCase++;
        }
        break;
    case 4:
        if (m_ramEncode->GetDistance() > 20) {
            m_ramMotor->Set(-0.3);
        } else {
            m_ramCase = 2;
            m_medRamCase = -1;
        }
        break;
    }
}

void RamGrab::Roll(roll_t direction)
{
    if (direction == kIn)
        m_roller->Set(1.0);
    else if (status == kOut)
        m_roller->Set(-1.0);
    else
        m_roller->Set(0.0);
}

void RamGrab::SetTimedOff(float timeout)
{
    m_rollTime->Stop();
    m_rollTime->Start();
    m_rollTime->Reset();

    m_rollTimeout = timeout;
}

void RamGrab::RollTime()
{
    if (m_rollTime->HasPeriodPassed(m_rollTimeout))
    {
        m_roller->Set(0.0);
        m_rollTime->Stop();
        m_rollTime->Reset();
    }
}

void RamGrab::Update()
{
    RamFire();
    MedRamFire();
    RollTime();
}
