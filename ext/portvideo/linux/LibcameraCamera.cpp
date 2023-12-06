#include "LibcameraCamera.h"
#include "CameraTool.h"

LibcameraCamera::LibcameraCamera(CameraConfig* cam_cfg) :
CameraEngine(cam_cfg) {
  // cam_buffer = NULL;
  // frm_buffer = NULL;
  // running = false;
  // buffers_initialized = false;
  cam_cfg->driver = DRIVER_DEFAULT;
}

LibcameraCamera::~LibcameraCamera(void) {
  CameraTool::saveSettings();
  // if ((pixelformat == V4L2_PIX_FMT_MJPEG) ||
  //     (pixelformat == V4L2_PIX_FMT_JPEG))
  //   tjDestroy(_jpegDecompressor);
  // if (cam_buffer!=NULL) delete []cam_buffer;
  // cam_buffer = NULL;
  // if (frm_buffer!=NULL) delete []frm_buffer;
  // frm_buffer = NULL;
}

// static methods --------------------------------------------------------------

int LibcameraCamera::getDeviceCount() {
  // ...
  return 0;
}

std::vector<CameraConfig> LibcameraCamera::getCameraConfigs(int dev_id=-1) {
  std::vector<CameraConfig> cfg_list;
  // ...
  return cfg_list;
}

CameraEngine* LibcameraCamera::getCamera(CameraConfig* cam_cfg) {
  // return new LibcameraCamera(cam_cfg);
  // ...
  return NULL;
}

// overridden pure virtual main methods from CameraEngine ----------------------

bool LibcameraCamera::initCamera() {
  return false;
}

bool LibcameraCamera::startCamera() {
  return false;
}

unsigned char* LibcameraCamera::getFrame() {
  return NULL;
}

bool LibcameraCamera::stopCamera() {
  return false;
}

bool LibcameraCamera::resetCamera() {
  return false;
}

bool LibcameraCamera::closeCamera() {
  return false;
}

bool LibcameraCamera::stillRunning() {
  return false;
}

// overridden pure virtual getters / setters from CameraEngine -----------------

int LibcameraCamera::getCameraSettingStep(int mode) {
  return 1;
}

bool LibcameraCamera::getCameraSettingAuto(int mode) {
  return false;
}

bool LibcameraCamera::setCameraSettingAuto(int mode, bool flag) {
  return false;
}

int LibcameraCamera::getCameraSetting(int mode) {
  return 1;
}

bool LibcameraCamera::setCameraSetting(int mode, int value) {
  return false;
}

int LibcameraCamera::getDefaultCameraSetting(int mode) {
  return 1;
}

bool LibcameraCamera::setDefaultCameraSetting(int mode) {
  return false;
}

int LibcameraCamera::getMaxCameraSetting(int mode) {
  return 1;
}

int LibcameraCamera::getMinCameraSetting(int mode) {
  return 0;
}

bool LibcameraCamera::hasCameraSetting(int mode) {
  return false
}

bool LibcameraCamera::hasCameraSettingAuto(int mode) {

}

// private ---------------------------------------------------------------------



