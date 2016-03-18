#include "headers.h"

#if __linux__   //  or #if __GNUC__
    #if __x86_64__ || __ppc64__
        #define ENVIRONMENT64
    #else
        #define ENVIRONMENT32
    #endif
#else
    #if _WIN32
        #define ENVIRONMENT32
    #else
        #define ENVIRONMENT64
    #endif
#endif // __linux__

#ifdef ENVIRONMENT64
    #define MAX_BLOCK_SIZE long long int
#else
    #define MAX_BLOCK_SIZE long int

#endif // ENVIRONMENT64

framebuffer::framebuffer() {
      // Open the file for reading and writing
      fbfd = open("/dev/fb0", O_RDWR);
      if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
      }
      printf("The framebuffer device was opened successfully.\n");

      // Get fixed screen information
      if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
      }

      // Get variable screen information
      if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
      }

      printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
      // Figure out the size of the screen in bytes
      screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

      // Map the device to memory
      fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
      if ((MAX_BLOCK_SIZE)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
      }
      printf("The framebuffer device was mapped to memory successfully.\n");
}

char* framebuffer::getfbp() { return fbp; }
   
struct fb_var_screeninfo framebuffer::getvinfo() { return vinfo; }

struct fb_fix_screeninfo framebuffer::getfinfo() { return finfo; }

void framebuffer::frameclose() {
      munmap(fbp, screensize);
      close(fbfd);
}
void framebuffer::render() {
	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	memcpy(this->fb_ptr, this->framebuffer_ptr, screensize);
	usleep(10000);
}

void framebuffer::clear() {
	long int screensize = this->vinfo.xres * this->vinfo.yres * this->vinfo.bits_per_pixel / 8;
	memset(framebuffer_ptr, 0, screensize);
}
