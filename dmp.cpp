#include "I2Cdev.h"
#include "headers\common.h"

#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
#include "SPI.h"
MPU6050 mpu;

// MPU control/status vars//
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
//ORIENTATION VARS//
Quaternion q, qlast, qavg;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
	mpuInterrupt = true;
}

void dmp_init(void) {
	// join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	Wire.begin();
	Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
	Fastwire::setup(400, true);
#endif
	// verify connection
	Serial.println(F("Testing device connections..."));
	Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
	// load and configure the DMP
	Serial.println(F("Initializing DMP..."));
	devStatus = mpu.dmpInitialize();
	mpu.setXAccelOffset(-5205); // 1688 factory default for my test chip
	mpu.setYAccelOffset(1749); // 1688 factory default for my test chip
	mpu.setZAccelOffset(1313); // 1688 factory default for my test chip
	mpu.setXGyroOffset(61);
	mpu.setYGyroOffset(6);
	mpu.setZGyroOffset(-35);
	if (devStatus == 0) {           // make sure it worked (returns 0 if so)
	  // turn on the DMP, now that it's ready
		Serial.println(F("Enabling DMP..."));
		mpu.setDMPEnabled(true);

		// enable Arduino interrupt detection
		Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
		attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
		mpuIntStatus = mpu.getIntStatus();

		// set our DMP Ready flag so the main loop() function knows it's okay to use it
		Serial.println(F("DMP ready! Waiting for first interrupt..."));
		dmpReady = true;

		// get expected DMP packet size for later comparison
		packetSize = mpu.dmpGetFIFOPacketSize();
	}
	else {
		// ERROR!
		// 1 = initial memory load failed
		// 2 = DMP configuration updates failed
		// (if it's going to break, usually the code will be 1)
		Serial.print(F("DMP Initialization failed (code "));
		Serial.print(devStatus);
		Serial.println(F(")"));
	}
}

void dmp_sample(int buf_YPR[][BUFFER_SIZE], int buf_WORLDACCEL[][BUFFER_SIZE], unsigned int &data_ptr) {
	// if programming failed, don't try to do anything
	if (!dmpReady) return;
	mpuInterrupt = false;                                               // reset interrupt flag and get INT_STATUS byte
	mpuIntStatus = mpu.getIntStatus();
	fifoCount = mpu.getFIFOCount();                                     // get current FIFO count
	if ((mpuIntStatus & 0x10) || fifoCount == 1024) {                   // check for overflow (this should never happen unless our code is too inefficient)
		mpu.resetFIFO();                                                // reset so we can continue cleanly                  *********fix*******
		Serial.println(F("FIFO overflow!"));
	}
	else if (mpuIntStatus & 0x02) {                                     // otherwise, check for DMP data ready interrupt (this should happen frequently)
		while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();    // wait for correct available data length, should be a VERY short wait
		mpu.getFIFOBytes(fifoBuffer, packetSize);                         // read a packet from FIFO
		fifoCount -= packetSize;                                          // track FIFO count here in case there is > 1 packet available

		//get quaternion and gravity
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		//store Euler angles in degrees
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);		//~14*
		//store real acceleration, adjusted to remove gravity and for orientation
		mpu.dmpGetAccel(&aa, fifoBuffer);
		mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);		//3*
		mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
		//place in ring buffers (YPR)
		data_ptr += 1;
		if (data_ptr == BUFFER_SIZE) data_ptr = 0;

		buf_YPR[0][data_ptr] = (ypr[0] * 180 / M_PI);
		buf_YPR[1][data_ptr] = (ypr[1] * 180 / M_PI);
		buf_YPR[2][data_ptr] = (ypr[2] * 180 / M_PI);
		//place in ring buffers (ACCEL)
		buf_WORLDACCEL[0][data_ptr] = (aaWorld.x);
		buf_WORLDACCEL[1][data_ptr] = (aaWorld.y);
		buf_WORLDACCEL[2][data_ptr] = ((aaWorld.z));
	}

}

void resetF(void) {
	mpu.setDMPEnabled(false);
}

void enableF(void) {
	mpu.setDMPEnabled(true);
}

