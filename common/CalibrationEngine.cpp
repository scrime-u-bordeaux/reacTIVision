/*  reacTIVision tangible interaction framework
	Copyright (C) 2005-2016 Martin Kaltenbrunner <martin@tuio.org>
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "CalibrationEngine.h"
#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#elif defined  LINUX
#include <unistd.h>
#endif

CalibrationEngine::CalibrationEngine(const char* out) {
	
	calibration = false;
	file_exists = false;
	quick_mode = true;
	
	if (strcmp(out, "none" ) == 0 ) {
#ifdef __APPLE__
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		CFStringGetCString( cfStringRef, path, 1024, kCFStringEncodingASCII);
		CFRelease( mainBundleURL);
		CFRelease( cfStringRef);
		sprintf(full_path,"%s/Contents/Resources/calibration.grid",path);
		calib_out = full_path;
#else
		calib_out = "./calibration.grid";
#endif
	} else calib_out = out;
	
	
#ifdef WIN32
	FILE *infile = fopen (calib_out, "r");
	if (infile != NULL) {
		file_exists = true;
		fclose (infile);
	}
#else
	file_exists = (access (calib_out, F_OK )==0);
#endif
	
	help_text.push_back( "CalibrationEngine:");
	help_text.push_back( "   c - toggle calibration");
	help_text.push_back( "   q - toggle quick/precise mode");
	help_text.push_back( "   u - reset selected point");
	help_text.push_back( "   j - reset calibration grid");
	help_text.push_back( "   l - revert to saved grid");
	help_text.push_back( "   r - show calibration result");
	help_text.push_back( "   a,d,w,x - move within grid");
	help_text.push_back( "   cursor keys - adjust grid point");
};

CalibrationEngine::~CalibrationEngine() {
	if (initialized) delete grid;
	
	if (file_exists) {
		remove(calib_bak);
	}
};

bool CalibrationEngine::init(int w, int h, int sb, int db) {
	
	FrameProcessor::init(w,h,sb,db);
	
	grid_size_x = 7;
	if (((float)width/height) > 1.3) grid_size_x +=2;
	if (((float)width/height) > 1.7) grid_size_x +=2;
	grid_size_y = 7;
	
	field_count_x = grid_size_x-1;
	field_count_y = grid_size_y-1;
	
	center_x = field_count_x/2;
	center_y = field_count_y/2;
	
	cell_size_x = width/field_count_x;
	cell_size_y = height/field_count_y;
	
	grid = new CalibrationGrid(grid_size_x,grid_size_y);
	grid->Reset();
	
	if (file_exists) {
		grid->Load(calib_out);
		sprintf(calib_bak,"%s.bak",calib_out);
		grid->Store(calib_bak);
	}
	
	grid_xpos = (char)(grid_size_x/2);
	grid_ypos = (char)(grid_size_y/2);
	
	return true;
}


bool CalibrationEngine::setFlag(unsigned char flag, bool value, bool lock) {
	return toggleFlag(flag,lock);
}

bool CalibrationEngine::toggleFlag(unsigned char flag, bool lock) {
	
	if (flag==KEY_C) {
		if (calibration) {
			calibration = false;
			if(ui) ui->setDisplayMode(prevMode);
		} else {
			calibration = true;
			
			grid_xpos = (char)(grid_size_x/2);
			grid_ypos = (char)(grid_size_y/2);

			if(ui) {
				prevMode = ui->getDisplayMode();
				ui->setDisplayMode(SOURCE_DISPLAY);
			}
		}
	}
	
	if(!calibration) return lock;
	
	if ((flag==KEY_T) || (flag==KEY_N))  {
		ui->setDisplayMode(SOURCE_DISPLAY);
	}
	
	if (flag==KEY_Q) {
		quick_mode=!quick_mode;
		grid_xpos=4;
		grid_ypos=3;
	} else if (flag==KEY_J) {
		grid->Reset();
	} else if (flag==KEY_U) {
		grid->Set(grid_xpos,grid_ypos,0.0,0.0);
	} else if (flag==KEY_L) {
		if (file_exists) grid->Load(calib_bak);
	}
	
	if (quick_mode) {
		if (flag==KEY_A) {
			grid_xpos-=center_x;
			if (grid_xpos<0) grid_xpos=0;
			if (grid_ypos!=center_y) grid_xpos=center_x;
		} else if (flag==KEY_D) {
			grid_xpos+=center_x;
			if (grid_xpos>field_count_x) grid_xpos=field_count_x;
			if (grid_ypos!=center_y) grid_xpos=center_x;
		} if (flag==KEY_W) {
			grid_ypos-=center_y;
			if (grid_ypos<0) grid_ypos=0;
			if (grid_ypos!=center_y) grid_xpos=center_x;
		} else if (flag==KEY_X) {
			grid_ypos+=center_y;
			if (grid_ypos>field_count_y) grid_ypos=field_count_y;
			if (grid_ypos!=center_y) grid_xpos=center_x;
		}
		
		//center
		if ((grid_xpos==center_x) && (grid_ypos==center_y)) {
			for (int gxp=0;gxp<grid_size_x;gxp++) {
				for (int gyp=0;gyp<grid_size_y;gyp++) {
					if (flag==KEY_LEFT) {
						GridPoint p = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,p.x-0.01,p.y);
					} else if (flag==KEY_RIGHT)  {
						GridPoint p = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,p.x+0.01,p.y);
					} else if (flag==KEY_UP) {
						GridPoint p = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,p.x,p.y-0.01);
					} else if (flag==KEY_DOWN) {
						GridPoint p = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,p.x,p.y+0.01);
					}
				}
			}
			// left & right
		} else if ((grid_xpos!=center_x) && (grid_ypos==center_y)) {
			for (int gxp=0;gxp<center_x;gxp++) {
				for (int gyp=0;gyp<grid_size_y;gyp++) {
					float step = 0.01*(center_x-gxp);
					if (grid_xpos==field_count_x) step=step*-1;
					
					if (flag==KEY_LEFT) {
						GridPoint pl = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,pl.x-step,pl.y);
						GridPoint pr = grid->GetInterpolated(field_count_x-gxp,gyp);
						grid->Set(field_count_x-gxp,gyp,pr.x+step,pr.y);
					} else if (flag==KEY_RIGHT) {
						GridPoint pu = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,pu.x+step,pu.y);
						GridPoint pr = grid->GetInterpolated(field_count_x-gxp,gyp);
						grid->Set(field_count_x-gxp,gyp,pr.x-step,pr.y);
					}
				}
			}
			// top & bottom
		} else if ((grid_xpos==center_x) && (grid_ypos!=center_y)) {
			for (int gxp=0;gxp<grid_size_x;gxp++) {
				for (int gyp=0;gyp<center_y;gyp++) {
					float step = 0.01*(center_y-gyp);
					if (grid_ypos==field_count_y) step=step*-1;
					
					if (flag==KEY_UP) {
						GridPoint pu = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,pu.x,pu.y-step);
						GridPoint pd = grid->GetInterpolated(gxp,field_count_y-gyp);
						grid->Set(gxp,field_count_y-gyp,pd.x,pd.y+step);
					} else if (flag==KEY_DOWN) {
						GridPoint pu = grid->GetInterpolated(gxp,gyp);
						grid->Set(gxp,gyp,pu.x,pu.y+step);
						GridPoint pd = grid->GetInterpolated(gxp,field_count_y-gyp);
						grid->Set(gxp,field_count_y-gyp,pd.x,pd.y-step);
					}
				}
			}
		}
		
	} else {
		if (flag==KEY_A) {
			grid_xpos--;
			if (grid_xpos<0) grid_xpos=0;
		} else if (flag==KEY_D) {
			grid_xpos++;
			if (grid_xpos>field_count_x) grid_xpos=field_count_x;
		} if (flag==KEY_W) {
			grid_ypos--;
			if (grid_ypos<0) grid_ypos=0;
		} else if (flag==KEY_X) {
			grid_ypos++;
			if (grid_ypos>field_count_y) grid_ypos=field_count_y;
		} else if (flag==KEY_LEFT) {
			GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
			grid->Set(grid_xpos,grid_ypos,p.x-0.01,p.y);
		} else if (flag==KEY_RIGHT)  {
			GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
			grid->Set(grid_xpos,grid_ypos,p.x+0.01,p.y);
		} else if (flag==KEY_UP) {
			GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
			grid->Set(grid_xpos,grid_ypos,p.x,p.y-0.01);
		} else if (flag==KEY_DOWN) {
			GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
			grid->Set(grid_xpos,grid_ypos,p.x,p.y+0.01);
		}
	}
	
	grid->Store(calib_out);
	return lock;
}

void CalibrationEngine::process(unsigned char *src, unsigned char *dest) {

	if(calibration) drawDisplay();
}

void CalibrationEngine::drawDisplay() {
	
	if (ui==NULL) return;
	if (ui->getDisplayMode()==NO_DISPLAY) return;

    float x,y;
    int gx,gy;
    GridPoint gp;

	// draw the circles
	ui->setColor(0, 0, 255);
	int offset = (width-height)/2;
	for (double a=-M_PI;a<M_PI;a+=0.005) {
		int half = height/2;

        // ellipse
        if (grid_size_x>7) {
            x = width/2+cos(a)*width/2;
            y = half+sin(a)*half;

            gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
            gx = (int)(x + gp.x*cell_size_x);
            gy = (int)(y + gp.y*cell_size_y);

           ui->drawPoint(gx, gy);
        }
        
        // extra circle
        if (grid_size_x>9) {
            x = offset+half+cos(a)*cell_size_x*4;
            y = half+sin(a)*half;
            
            gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
            gx = (int)(x + gp.x*cell_size_x);
            gy = (int)(y + gp.y*cell_size_y);
            
            ui->drawPoint(gx, gy);
        }
		
		// outer circle
		x = offset+half+cos(a)*cell_size_x*3;
		y = half+sin(a)*half;
		
		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		ui->drawPoint(gx, gy);

		// middle circle
		x = offset+half+cos(a)*cell_size_x*2;
		y = half+sin(a)*(half-cell_size_y);
		
		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		ui->drawPoint(gx, gy);

		// inner circle
		x = offset+half+cos(a)*cell_size_x;
		y = half+sin(a)*cell_size_y;
		
		gp = grid->GetInterpolated(x/cell_size_x,y/cell_size_y);
		gx = (int)(x + gp.x*cell_size_x);
		gy = (int)(y + gp.y*cell_size_y);

		ui->drawPoint(gx, gy);
	}
	
	// draw the horizontal lines
	ui->setColor(255, 255, 255);
	for (int i=0;i<grid_size_y;i++) {
		float start_x = 0;
		float start_y = i*cell_size_y;
		float end_x = field_count_x*cell_size_x;
		float end_y = start_y;
		
		for (float lx=start_x;lx<end_x;lx++) {
			float ly = end_y - (end_y-start_y)/(end_x-start_x)*(end_x-lx);
			
			GridPoint gp = grid->GetInterpolated(lx/cell_size_x,ly/cell_size_y);
			int gx = (int)(lx+gp.x*cell_size_x);
			int gy = (int)(ly+gp.y*cell_size_y);

			ui->drawPoint(gx, gy);
		}
	}
	
	// draw the vertical lines
	for (int i=0;i<grid_size_x;i++) {
		
		float start_x = i*cell_size_x;
		float start_y = 0;
		float end_x = start_x;
		float end_y = field_count_y*cell_size_y;
		
		for (float ly=start_y;ly<end_y;ly++) {
			float lx = end_x - (end_x-start_x)/(end_y-start_y)*(end_y-ly);
			
			GridPoint gp = grid->GetInterpolated(lx/cell_size_x,ly/cell_size_y);
			int gx = (int)(lx+gp.x*cell_size_x);
			int gy = (int)(ly+gp.y*cell_size_y);
			
			ui->drawPoint(gx, gy);
		}
	}
	
	// draw the red box for the selected point
	ui->setColor(255, 0, 80);
	GridPoint p = grid->GetInterpolated(grid_xpos,grid_ypos);
	gx = (int)(grid_xpos*cell_size_x + p.x*cell_size_x);
	gy = (int)(grid_ypos*cell_size_y + p.y*cell_size_y);

	ui->fillRect(gx-3, gy-3,7,7);
	
	// draw the grid points
	ui->setColor(255, 0, 255);
	for (int i=0;i<grid_size_x;i++) {
		for (int j=0;j<grid_size_y;j++) {
			GridPoint gp = grid->GetInterpolated(i,j);
			int gx = (int)(i*cell_size_x + gp.x*cell_size_x);
			int gy = (int)(j*cell_size_y + gp.y*cell_size_y);

			ui->fillRect(gx-1, gy-1,3,3);
		}
	}
	
}


