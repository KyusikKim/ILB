#include "fbc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>	
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <time.h>	

void fbc_debug(const char id[], const char msg[])
{
	if(id == NULL || msg == NULL)
		return;

	printf("%s, %s\n", id, msg);
}
	
void fbc_debug_time(const char id[])
{
	struct timespec time_toPrint;
	
	if(id == NULL)
		return;

	clock_gettime(CLOCK_REALTIME, &time_toPrint);
	//printf("%s : %ld.%ld sec.\n", id, time_toPrint.tv_sec, time_toPrint.tv_nsec);
	printf("%ld.%ld\n", time_toPrint.tv_sec, time_toPrint.tv_nsec);
}

#ifdef FBC_ENABLED
bool fbc_store(char url[])
{
	struct fb_var_screeninfo fbinfo_variable;

	int dev_fb;
	int file_fbcache;

	char path_fbcache[FBC_MAX_PATH];

	int mapping_size_dev_fb;
	char *mapping_addr_dev_fb = NULL;

	int result_op;

	if(url == NULL)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to get valid url.");
#endif
		return false;
	}

	if((dev_fb = open(FBC_DEV_FB, O_RDWR)) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to open framebuffer device.");
#endif
		return false;
	}

	if(ioctl(dev_fb, FBIOGET_VSCREENINFO, &fbinfo_variable) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to get variable screen information.");
#endif
		close(dev_fb);

		return false;
	}

	mapping_size_dev_fb = fbinfo_variable.xres * fbinfo_variable.yres * fbinfo_variable.bits_per_pixel / 8;

	if((mapping_addr_dev_fb = (char *)mmap(0, mapping_size_dev_fb, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fb, 0)) == MAP_FAILED) 
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to mmap frame buffer device.");
#endif
		close(dev_fb);

		return false;
	}

	mkdir(FBC_DIR_FBCACHE, 0777);
	
	sprintf(path_fbcache, "%s/%s", FBC_DIR_FBCACHE, url); 

	if((file_fbcache = open(path_fbcache, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to make new image file.");
#endif
		close(dev_fb);

		return false;
	}

	if((result_op = write(file_fbcache, mapping_addr_dev_fb, mapping_size_dev_fb)) != mapping_size_dev_fb)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "Our memory-mapped area size did not equal written size.");
#endif
	}

	if((munmap(mapping_addr_dev_fb, mapping_size_dev_fb)) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to munmap frame buffer device.");
#endif
		close(dev_fb);
		close(file_fbcache);

		return false;
	}

	close(dev_fb);
	close(file_fbcache);

	return true;
}

bool fbc_restore(char url[])
{
	struct fb_var_screeninfo fbinfo_variable;

	int dev_fb;
	int file_fbcache;

	char path_fbcache[FBC_MAX_PATH];
	struct stat info_fbcache;

	int mapping_size_dev_fb, mapping_size_fbcache;
	char *mapping_addr_dev_fb = NULL, *mapping_addr_fbcache = NULL;

	if(url == NULL)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to get valid url.");
#endif
		return false;
	}

	sprintf(path_fbcache, "%s/%s", FBC_DIR_FBCACHE, url); 

	if((file_fbcache = open(path_fbcache, O_RDWR)) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to open fbcache file.");
		fbc_debug(" - its path : ", path_fbcache);
#endif

		return false;
	}

	if(fstat(file_fbcache, &info_fbcache))
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to get information of fbcache.");
#endif
		close(file_fbcache);

		return false;
	}

	mapping_size_fbcache = info_fbcache.st_size;

	if((dev_fb = open(FBC_DEV_FB, O_RDWR)) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to open framebuffer device.");
#endif
		close(file_fbcache);

		return false;
	}

	if(ioctl(dev_fb, FBIOGET_VSCREENINFO, &fbinfo_variable) < 0)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to get variable screen information.");
#endif
		close(dev_fb);
		close(file_fbcache);

		return false;
	}

	if((mapping_addr_fbcache = (char *)mmap(0, mapping_size_fbcache, PROT_READ, MAP_SHARED, file_fbcache, 0)) == MAP_FAILED) 
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to mmap fbcache.");
#endif
		close(dev_fb);
		close(file_fbcache);

		return false;
	}

	mapping_size_dev_fb = fbinfo_variable.xres * fbinfo_variable.yres * fbinfo_variable.bits_per_pixel / 8;	

	if((mapping_addr_dev_fb = (char *)mmap(0, mapping_size_dev_fb, PROT_WRITE, MAP_SHARED, dev_fb, 0)) == MAP_FAILED) 
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to mmap frame buffer device.");
#endif
		munmap(mapping_addr_fbcache, mapping_size_fbcache);
		close(dev_fb);
		close(file_fbcache);

		return false;
	}

	if(memcpy(mapping_addr_dev_fb, mapping_addr_fbcache, mapping_size_fbcache) == NULL)
	{
#ifdef FBC_DEBUG_ENABLED
		fbc_debug(__func__, "We failed to overwrite frame buffer device.");
#endif
		munmap(mapping_addr_dev_fb, mapping_size_dev_fb);
		munmap(mapping_addr_fbcache, mapping_size_fbcache);
		close(dev_fb);
		close(file_fbcache);

		return false;
	}

	munmap(mapping_addr_dev_fb, mapping_size_dev_fb);
	munmap(mapping_addr_fbcache, mapping_size_fbcache);

	close(dev_fb);
	close(file_fbcache);

	return true;
}
#endif

