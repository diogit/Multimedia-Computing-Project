
#include "ofxiOSCoreMotion.h"


ofxiOSCoreMotion::ofxiOSCoreMotion() {
    
    motionManager = [[CMMotionManager alloc] init];
    referenceAttitude = nil;
    referenceFrameType = [motionManager attitudeReferenceFrame]; // default = 1,CMAttitudeReferenceFrameXArbitraryZVertical
    updateFrequency = 1.0f/60.0f;
    roll = pitch = yaw = 0;
    enableAttitude = false;
    enableGyro = false;
    enableAccelerometer = false;
    enableMagnetometer = false;
}


ofxiOSCoreMotion::~ofxiOSCoreMotion() {
    
    [referenceAttitude release];
    referenceAttitude = nil;
    [motionManager stopAccelerometerUpdates];
    [motionManager stopGyroUpdates];
    [motionManager stopMagnetometerUpdates];
    [motionManager stopDeviceMotionUpdates];
    [motionManager release];
    motionManager = nil;
}

void ofxiOSCoreMotion::setup(bool enableAttitude, bool enableAccelerometer, bool enableGyro, bool enableMagnetometer) {
    
    if(enableAttitude) setupAttitude();
    if(enableAccelerometer) setupAccelerometer();
    if(enableGyro) setupGyroscope();
    if(enableMagnetometer) setupMagnetometer();
}

void ofxiOSCoreMotion::setupAttitude(CMAttitudeReferenceFrame type) {
    
    enableAttitude = true;
    referenceFrameType = type;
    
    [motionManager setDeviceMotionUpdateInterval: updateFrequency];
    
    // default is CMAttitudeReferenceFrameXArbitraryZVertical
    // [motionManager startDeviceMotionUpdates];
    [motionManager startDeviceMotionUpdatesUsingReferenceFrame:referenceFrameType];
    
}

void ofxiOSCoreMotion::disableAttitude() {
    
    [motionManager stopDeviceMotionUpdates];
    enableAttitude = false;
    roll = pitch = yaw = 0;
    attitudeQuat.set(0, 0, 0, 1);
    rotMatrix.makeIdentityMatrix();
    gravity.set(0,0,0);
    userAcceleration.set(0, 0, 0);
    magneticField.set(0, 0, 0);
    
}

void ofxiOSCoreMotion::setupAccelerometer() {
    
    enableAccelerometer = true;
    [motionManager setAccelerometerUpdateInterval:updateFrequency];
    [motionManager startAccelerometerUpdates];
}

void ofxiOSCoreMotion::disableAccelerometer() {
    
    [motionManager stopAccelerometerUpdates];
    enableAccelerometer = false;
    accelerometerData.set(0, 0, 0);
}

void ofxiOSCoreMotion::setupGyroscope() {
    
    enableGyro = true;
    [motionManager setGyroUpdateInterval:updateFrequency];
    [motionManager startGyroUpdates];
}

void ofxiOSCoreMotion::disableGyroscope() {
    
    [motionManager stopGyroUpdates];
    enableGyro = false;
    gyroscopeData.set(0, 0, 0);
}

void ofxiOSCoreMotion::setupMagnetometer() {
    
    enableMagnetometer = true;
    [motionManager setMagnetometerUpdateInterval:updateFrequency];
    [motionManager startMagnetometerUpdates];
}

void ofxiOSCoreMotion::disableMagnetometer() {
    
    [motionManager stopMagnetometerUpdates];
    enableMagnetometer = false;
    magnetometerData.set(0, 0, 0);
}

void ofxiOSCoreMotion::setUpdateFrequency(float updateFrequency) {
    
    // default = 1.0f/ofGetFrameRate();
    this->updateFrequency = updateFrequency;
    if(enableAttitude) [motionManager setDeviceMotionUpdateInterval: updateFrequency];
    if(enableAccelerometer) [motionManager setAccelerometerUpdateInterval:updateFrequency];
    if(enableGyro) [motionManager setGyroUpdateInterval:updateFrequency];
    if(enableMagnetometer) [motionManager setMagnetometerUpdateInterval:updateFrequency];
}


// resets reference attitude to current or null's the ref frame
void ofxiOSCoreMotion::resetAttitude(bool toCurrentReferenceFrame) {
    
    if(toCurrentReferenceFrame) {
        CMDeviceMotion *deviceMotion = motionManager.deviceMotion;
        CMAttitude *attitude = deviceMotion.attitude;
        referenceAttitude = [attitude retain];
    } else {
        if(referenceAttitude != nil) {
            [referenceAttitude release];
            referenceAttitude = nil;
        }
    }
}


// convenience method to update all objc properties to OF friendly at once
void ofxiOSCoreMotion::update() {
    
    CMDeviceMotion *deviceMotion = motionManager.deviceMotion;
    
    
    if(enableAttitude) {
    
        // gravity
        gravity.x = deviceMotion.gravity.x;
        gravity.y = deviceMotion.gravity.y;
        gravity.z = deviceMotion.gravity.z;
        
        // user acceleration
        userAcceleration.x = deviceMotion.userAcceleration.x;
        userAcceleration.y = deviceMotion.userAcceleration.y;
        userAcceleration.z = deviceMotion.userAcceleration.z;
        
        // magnetic field
        magneticField.x = deviceMotion.magneticField.field.x;
        magneticField.y = deviceMotion.magneticField.field.y;
        magneticField.z = deviceMotion.magneticField.field.z;
        
        CMAttitude *attitude = deviceMotion.attitude;
        if (referenceAttitude != nil ) {
            [attitude multiplyByInverseOfAttitude:referenceAttitude];
        }
        
                
        // attitude euler angles
        roll = attitude.roll;
        pitch = attitude.pitch;
        yaw	= attitude.yaw;
        
        
        // attitude quaternion
        CMQuaternion quat = attitude.quaternion;
        attitudeQuat.set(quat.x, quat.y, quat.z, quat.w);

        // attitude rotation matrix
        CMRotationMatrix rot = attitude.rotationMatrix;
        rotMatrix.set(rot.m11, rot.m21, rot.m31, 0,
                      rot.m12, rot.m22, rot.m32, 0,
                      rot.m13, rot.m23, rot.m33, 0,
                      0, 0, 0, 1);
    }
    
    if(enableAccelerometer) {
        CMAccelerometerData* acc = motionManager.accelerometerData;        
        accelerometerData.x = acc.acceleration.x;
        accelerometerData.y = acc.acceleration.y;
        accelerometerData.z = acc.acceleration.z;
    }
    
    if(enableGyro) {
        CMGyroData* gyro = motionManager.gyroData;
        gyroscopeData.x = gyro.rotationRate.x;
        gyroscopeData.y = gyro.rotationRate.y;
        gyroscopeData.z = gyro.rotationRate.z;
    }
    
    if(enableMagnetometer) {
        CMMagnetometerData *mag = motionManager.magnetometerData;
        magnetometerData.x = mag.magneticField.x;
        magnetometerData.y = mag.magneticField.y;
        magnetometerData.z = mag.magneticField.z;
    }
}

ofVec3f ofxiOSCoreMotion::getAccelerometerData() {
    return accelerometerData;
}

ofVec3f ofxiOSCoreMotion::getGyroscopeData() {
    return gyroscopeData;
}

ofVec3f ofxiOSCoreMotion::getMagnetometerData() {
    return magnetometerData;
}

float ofxiOSCoreMotion::getRoll() {
    return roll;
}

float ofxiOSCoreMotion::getPitch() {
    return pitch;
}

float ofxiOSCoreMotion::getYaw() {
    return yaw;
}

ofQuaternion ofxiOSCoreMotion::getQuaternion() {
    return attitudeQuat;
}

ofMatrix4x4 ofxiOSCoreMotion::getRotationMatrix() {
    return rotMatrix;
}

ofVec3f ofxiOSCoreMotion::getGravity() {
    return gravity;
}

ofVec3f ofxiOSCoreMotion::getUserAcceleration() {
    return userAcceleration;
}

ofVec3f ofxiOSCoreMotion::getMagneticField() {
    return magneticField;
}
