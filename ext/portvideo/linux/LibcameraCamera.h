#ifndef LibcameraCamera_H
#define LibcameraCamera_H

#include <unistd.h>
#include "CameraEngine.h"
#include <libcamera.h>

class LibcameraCamera : public CameraEngine
{
public:
  LibcameraCamera(CameraConfig* cam_cfg);
	~LibcameraCamera();
	
	static int getDeviceCount();
	static std::vector<CameraConfig> getCameraConfigs(int dev_id=-1);
	static CameraEngine* getCamera(CameraConfig* cam_cfg);
	
	bool initCamera() override;
	bool startCamera() override;
	unsigned char* getFrame() override;
	bool stopCamera() override;
	bool resetCamera() override;
	bool closeCamera() override;
	bool stillRunning() override;
	
	int getCameraSettingStep(int mode) override;
  bool getCameraSettingAuto(int mode) override;
	bool setCameraSettingAuto(int mode, bool flag) override;
	int getCameraSetting(int mode) override;
	bool setCameraSetting(int mode, int value) override;
  int getDefaultCameraSetting(int mode) override;
  bool setDefaultCameraSetting(int mode) override;
	int getMaxCameraSetting(int mode) override;
	int getMinCameraSetting(int mode) override;
  bool hasCameraSetting(int mode) override;
  bool hasCameraSettingAuto(int mode) override;

private:
  // libcamera stuff here
};

#endif