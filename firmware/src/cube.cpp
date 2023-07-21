#include "cube.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>

static MPU6050 mpu;
static uint8_t fifoBuffer[64]; // FIFO storage buffer
                               // orientation/motion vars
static Quaternion q;           // [w, x, y, z]         quaternion container
static VectorFloat gravity;    // [x, y, z]            gravity vector
static float euler[3];         // [psi, theta, phi]    Euler angle container
static float temp[3];          // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void Cube::init()
{
    delta = 10;

    Wire.begin(19, 18);
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    //// supply your own gyro offsets here, scaled for min sensitivity
    // mpu.setXGyroOffset(-1);
    // mpu.setYGyroOffset(-10);
    // mpu.setZGyroOffset(64);
    // mpu.setZAccelOffset(816);

    if (0 == devStatus)
    {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);
    }
}
bool Cube::isSuccessed()
{
    return (0 == devStatus);
}

bool Cube::readYawPitchRoll()
{
    if (0 != devStatus)
        return false;

    // read a packet from FIFO
    // Get the Latest packet
    if (0 == mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        return false;

    // return Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(temp, &q, &gravity);
    ypr[0] = temp[0] * 180 / M_PI;
    ypr[1] = temp[1] * 180 / M_PI;
    ypr[2] = temp[2] * 180 / M_PI;

    return true;
}

int Cube::getYaw()
{
    return ypr[0];
}

int Cube::getPitch()
{
    return ypr[1];
}

int Cube::getRoll()
{
    return ypr[2];
}

bool Cube::isWorkFace()
{
    int t = 0;

    t = getRoll();
    if (t > 90 + delta || t < 90 - delta)
        return false;

    return true;
}

bool Cube::isIdleFace()
{
    int t = 0;

    t = getRoll();
    if (t > 0 + delta || t < 0 - delta)
        return false;

    return true;
}

bool Cube::isBreakFace()
{
    int t = 0;

    t = getRoll();
    if (t > -90 + delta || t < -90 - delta)
        return false;

    return true;
}

int Cube::currentFaceId()
{
    if (isWorkFace())
        return 2;
    else if (isIdleFace())
        return 3;
    else if (isBreakFace())
        return 5;

    return 0;
}